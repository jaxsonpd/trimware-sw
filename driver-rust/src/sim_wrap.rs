/// A wrapper around the sim connect interface to allow easy reuse
/// 
/// Author: Jack Duignan (JackpDuignan@gmail.com)

use std::{any::Any, collections::HashMap, error::Error, pin::Pin, sync::mpsc::{channel, Receiver}};
use std::fmt::Debug;

use msfs::{sim_connect::{self, DataDefinition, Period, SimConnect, SimConnectRecv}, sys::{self, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_RECV_SIMOBJECT_DATA}};

/// Hold sim events and their ids for later use 
#[derive(Debug)]
pub struct EventIdTable {
    map: HashMap<String, u32>,
}

impl EventIdTable {
    pub fn new() -> Self {
        EventIdTable { map: HashMap::new() }
    }

    pub fn add(&mut self, name: String, id: u32) {
        self.map.insert(name, id);
    }

    pub fn get(&mut self, name: String) -> Option<u32> {
        match self.map.get(&name) {
            Some(value) => return Some(*value),
            None => return None
        }
    }
}

pub trait SimDataObject: Debug {
    /// Register a request with the simulator to get the data.
    /// 
    /// sim - the simulator connection
    /// id - the id of the request
    /// 
    /// returns an error if it cannot register the request
    // fn register_request(&self, sim: &mut Pin<Box<SimConnect<'static>>>, id: u32) -> Result<(), msfs::sim_connect::HResult>;

    /// Get event data from the sim
    /// 
    /// sim - the simulator connection
    /// event - the event to fetch
    /// 
    /// returns an error if it cannot get the data
    fn get_event_data(&self, sim: &mut msfs::sim_connect::SimConnect<'_>, event: &SIMCONNECT_RECV_SIMOBJECT_DATA) -> Option<Box<dyn SimDataObject>>;

    fn as_any(&self) -> &dyn Any;
}


#[derive(Debug)]
pub struct SimWrapper {
    sim_object: Pin<Box<sim_connect::SimConnect<'static>>>,
    event_table: EventIdTable,
    data_rx: Receiver<Box<dyn SimDataObject>>
}

impl SimWrapper {
    /// Create a new simwrapper and attempt to connect to the sim.
    /// 
    /// name - the name of the sim connection
    /// data_objects - the objects to fetch data from
    pub fn new(name: String, data_objects: Vec<Box<dyn SimDataObject>>) -> Result<Self, Box<dyn std::error::Error>> {
        let (data_tx, data_rx) = channel::<Box<dyn SimDataObject>>();

        let sim = match SimConnect::open(name.as_str(), move | sim: &mut SimConnect<'_>, recv: SimConnectRecv | {
            match recv {
                SimConnectRecv::SimObjectData(event) => {
                    match data_objects[event.dwRequestID as usize].get_event_data(sim, event) {
                        Some(data) => {let _ = data_tx.send(data);}
                        None => {return;}
                    }
                },
                _ => println!("{:?}", recv),
            }
        } ) {
                Ok(sim) => sim,
                Err(e) => {
                    return Err(Box::new(e));
                }
            };

        Ok(SimWrapper { 
            sim_object: sim,
            data_rx: data_rx,
            event_table: EventIdTable::new()
        })
    }

    /// Register a data request to get data from
    pub fn register_data_request<T: DataDefinition>(&mut self, id: u32) -> Result<(), Box<dyn Error>> {
        self.sim_object.request_data_on_sim_object::<T>(id, SIMCONNECT_OBJECT_ID_USER, Period::SimFrame)?;

        Ok(())
    }

    /// Poll the simulator for more data, this function doesn't return data
    /// use get_data for this
    pub fn poll_sim(&mut self) -> Result<(), Box<dyn Error>>{
        self.sim_object.call_dispatch()?;
        Ok(())
    }

    /// Get one new bit of data from the simulator
    /// 
    /// # Returns
    /// data if available
    pub fn get_data(&mut self) -> Option<Box<dyn SimDataObject>>{
        match self.data_rx.try_recv() {
            Ok(received_data) => {
                return Some(received_data)
            },
            Err(_) => {return None;}
        }
    }

    /// Register a new event with the simulator
    ///
    /// # Arguments
    /// - sim the sim to affect
    /// - event_name the text string event name
    ///
    /// # Returns
    /// - The instance specific event number
    fn add_sim_event(sim: &mut Pin<Box<SimConnect<'static>>>, event_name: String) -> Result<u32, Box<dyn std::error::Error>> {
        let event_id: u32 = match sim.map_client_event_to_sim_event(event_name.as_str(), false) {
            Ok(id) => id,
            Err(err) => {
                return Err(Box::new(err));
            }
        };

        Ok(event_id)
    }

    /// Add a new event to the wrapper that can be then
    /// set.
    /// 
    /// name - the name of the event to add (must match a simconnect event)
    pub fn register_event(&mut self, name: String) -> Result<(), Box<dyn std::error::Error>> {
        match SimWrapper::add_sim_event(&mut self.sim_object, name.clone()) {
            Ok(id) => {
                self.event_table.add(name, id);
            },
            Err(e) => {
                return Err(e);
            }
        };
            
        Ok(())
    }

    /// Write to an event
    /// 
    /// name - the name of the event to write to needs to have been added first.
    pub fn write_event(&mut self, name: String, value: sys::DWORD) -> Result<(), Box<dyn std::error::Error>> {
        let event_num = match self.event_table.get(name.clone()) {
            Some(num) => num,
            None => return Err(Box::from(format!("event {} not in current table try registering it", name)))
        };

        self
            .sim_object
            .transmit_client_event(SIMCONNECT_OBJECT_ID_USER, event_num as sys::DWORD, value)?;

        Ok(())
    }
}




mod test {
    use super::*;

    #[test]
    fn test_event_table_create() {
        let table = EventIdTable::new();
        assert_eq!(table.map.len(), 0);
    }

    #[test]
    fn test_event_table_write_read() {
        let mut table = EventIdTable::new();
        table.add("TEST".to_string(), 1);
        assert_eq!(table.get("TEST".to_string()), Some(1));
    }

    #[test]
    fn test_event_table_overwrite_read() {
        let mut table = EventIdTable::new();
        table.add("TEST".to_string(), 1);
        table.add("TEST".to_string(), 2);
        assert_eq!(table.get("TEST".to_string()), Some(2));
    }

    #[test]
    fn test_event_table_no_write_read() {
        let mut table = EventIdTable::new();
        assert_eq!(table.get("TEST".to_string()), None);
    }
}