/// A wrapper around the sim connect interface to allow easy reuse
/// 
/// Author: Jack Duignan (JackpDuignan@gmail.com)

use std::{any::Any, collections::HashMap, error::Error, pin::Pin, sync::mpsc::{channel, Receiver}};
use std::fmt::Debug;

use msfs::{sim_connect::{self, data_definition, DataDefinition, Period, SimConnect, SimConnectRecv}, sys::{self, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_RECV_SIMOBJECT_DATA}};

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

trait SimDataObject: Debug {
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


#[data_definition]
#[derive(Debug, Clone)]
pub struct FrequencyData {
    #[name = "COM ACTIVE FREQUENCY:1"]
    #[unit = "MHz"]
    com1_active: f64,
    #[name = "COM STANDBY FREQUENCY:1"]
    #[unit = "MHz"]
    com1_standby: f64,
}

impl SimDataObject for FrequencyData {
    // fn register_request(&self, sim: &mut Pin<Box<SimConnect<'static>>>, id: u32) -> Result<(), msfs::sim_connect::HResult> {
    //     sim.request_data_on_sim_object::<FrequencyData>(id, SIMCONNECT_OBJECT_ID_USER, Period::SimFrame);

    //     Ok(())
    // }

    fn get_event_data(
        &self,
        sim: &mut msfs::sim_connect::SimConnect<'_>,
        event: &SIMCONNECT_RECV_SIMOBJECT_DATA
    ) -> Option<Box<dyn SimDataObject>> {
        event
            .into::<FrequencyData>(sim)
            .map(|data| Box::new(data.clone()) as Box<dyn SimDataObject>)
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}

#[derive(Debug)]
pub struct SimWrapper {
    sim_object: Pin<Box<sim_connect::SimConnect<'static>>>,
    // data_objects: Vec<Box<dyn SimDataObject>>,
    event_table: EventIdTable,
    data_rx: Receiver<Box<dyn SimDataObject>>
}

impl SimWrapper {
    /// Create a new simwrapper and attempt to connect to the sim.
    /// 
    /// name - the name of the sim connection
    /// data_objects - the objects to fetch data from
    fn new(name: String, data_objects: Vec<Box<dyn SimDataObject>>) -> Result<Self, Box<dyn std::error::Error>> {
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
            Ok(received_data) => {return Some(received_data)},
            Err(e) => {return None;}
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

#[derive(Debug)]
pub enum FrequencyName {
    Com1Active,
    Com1Standby,
    Com2Active,
    Com2Standby,
    Nav1Active,
    Nav1Standby,
    Nav2Active,
    Nav2Standby,
    XPDR,
}

impl FrequencyName {
    /// Return the simconnect string name of the frequency
    fn as_event(&self) -> String {
        match self {
            FrequencyName::Com1Active => "COM_RADIO_SET_HZ".to_string(),
            FrequencyName::Com1Standby => "COM_STBY_RADIO_SET_HZ".to_string(),
            FrequencyName::Com2Active => "COM2_RADIO_SET_HZ".to_string(),
            FrequencyName::Com2Standby => "COM2_STBY_RADIO_SET_HZ".to_string(),
            FrequencyName::Nav1Active => "NAV1_RADIO_SET_HZ".to_string(),
            FrequencyName::Nav1Standby => "NAV1_STBY_SET_HZ".to_string(),
            FrequencyName::Nav2Active => "NAV2_RADIO_SET_HZ".to_string(),
            FrequencyName::Nav2Standby => "NAV2_STBY_SET_HZ".to_string(),
            FrequencyName::XPDR => "XPNDR_SET".to_string()
        }
    }

    /// Get all of the enum
    pub fn all() -> &'static [FrequencyName] {
        use FrequencyName::*;
        &[
            Com1Active,
            Com1Standby,
            Com2Active,
            Com2Standby,
            Nav1Active,
            Nav1Standby,
            Nav2Active,
            Nav2Standby,
            XPDR,
        ]
    }
}

#[derive(Debug)]
pub struct SimFreq {
    wrapper: SimWrapper
} 

impl SimFreq {
    /// Create a new object to comunicate with the sim regarding frequencies
    pub fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let data_objects: Vec<Box<dyn SimDataObject>> = vec![Box::new(FrequencyData{com1_active: 118.000, com1_standby:118.000})];
        let mut wrapper = SimWrapper::new("Frequency Communication".to_string(), data_objects)?;

        wrapper.register_data_request::<FrequencyData>(0)?;
        
        for event_name in FrequencyName::all() {
            wrapper.register_event(event_name.as_event())?;
        }

        Ok(SimFreq { wrapper: wrapper })
    }

    /// Set a specific frequency
    /// 
    /// freq_name - the frequency to set
    /// value - the frequency to set
    pub fn set(&mut self, freq_name: FrequencyName, value: u32) -> Result<(), Box<dyn std::error::Error>>{
        self.wrapper.write_event(freq_name.as_event(), value)?;

        Ok(())
    }

    /// Get frequency updates if they have occured
    /// 
    /// # Returns
    /// A frequency update if it has occured
    pub fn get_freq_update(&mut self) -> Option<FrequencyData> {
        self.wrapper.poll_sim();

        match self.wrapper.get_data() {
            Some(data) => {
                if let Some(freq) = data.as_any().downcast_ref::<FrequencyData>() {
                    println!("{:?}", freq);
                    return Some(freq.clone());
                }
            },
            None => return None
        }

        None
    }
}


mod test {
    use std::time::{Instant, Duration};

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

    // #[cfg(feature = "sim_tests")]
    #[test]
    fn test_SimFreq_new() {
        let freq = SimFreq::new().expect("Check that the sim is open");
    }

    // #[cfg(feature = "sim_tests")]
    #[test]
    fn test_SimFreq_set() {
        let mut freq = SimFreq::new().expect("Check that the sim is open");
        std::thread::sleep(std::time::Duration::from_secs(5));
        freq.set(FrequencyName::Com1Standby, 129000000).expect("bad error");
    }

    // #[cfg(feature = "sim_tests")]
    #[test]
    fn test_SimWrapper_poll() {
        let data_objects: Vec<Box<dyn SimDataObject>> = vec![Box::new(FrequencyData{com1_active: 118.000, com1_standby:118.000})];
        let mut wrapper = SimWrapper::new("Frequency Communication".to_string(), data_objects).expect("Failed to open connection");

        let start_time = std::time::SystemTime::now();

        let duration = Duration::from_secs(5);
        let start_time = Instant::now();

        while Instant::now().duration_since(start_time) < duration {
            wrapper.poll_sim().expect("Failed to poll");
        }
    }
}