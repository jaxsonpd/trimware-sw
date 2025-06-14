/// A wrapper around the sim connect interface to allow easy reuse
/// 
/// Author: Jack Duignan (JackpDuignan@gmail.com)

use msfs::{sim_connect::{self, data_definition, Period, SimConnect}, sys::{self, SIMCONNECT_OBJECT_ID, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_RECV_SIMOBJECT_DATA}};

use core::{error, num};
use std::{collections::{hash_map, HashMap}, error::Error, f32::consts::E, pin::Pin};

/// Hold sim events and their ids for later use 
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

    pub fn get(&mut self, name: String) -> Option<&u32> {
        self.map.get(&name)
    }
}

trait SimDataObject {
    /// Register a request with the simulator to get the data.
    /// 
    /// sim - the simulator connection
    /// 
    /// returns an error if it cannot register the request
    fn register_request(&self, sim: &mut Pin<Box<SimConnect<'static>>>, id: u32) -> Result<(), msfs::sim_connect::HResult>;

    /// Get event data from the sim
    /// 
    /// event - the event to fetch
    fn get_event_data(&self, sim: &mut Pin<Box<SimConnect<'static>>>, event: &SIMCONNECT_RECV_SIMOBJECT_DATA) -> Option<Box<dyn SimDataObject>>;
}


#[data_definition]
#[derive(Debug, Clone)]
struct FrequencyData {
    #[name = "COM STANDBY FREQUENCY:1"]
    #[unit = "MHz"]
    com1_standby: f64,
    #[name = "COM ACTIVE FREQUENCY:1"]
    #[unit = "MHz"]
    com1_active: f64,
}

impl SimDataObject for FrequencyData {
    fn register_request(&self, sim: &mut Pin<Box<SimConnect<'static>>>, id: u32) -> Result<(), msfs::sim_connect::HResult> {
        sim.request_data_on_sim_object::<FrequencyData>(id, SIMCONNECT_OBJECT_ID_USER, Period::SimFrame);

        Ok(())
    }

    fn get_event_data(
        &self,
        sim: &mut Pin<Box<SimConnect<'static>>>,
        event: &SIMCONNECT_RECV_SIMOBJECT_DATA
    ) -> Option<Box<dyn SimDataObject>> {
        event
            .into::<FrequencyData>(sim)
            .map(|data| Box::new(data.clone()) as Box<dyn SimDataObject>)
    }
}


pub struct SimWrapper {
    sim_object: Pin<Box<sim_connect::SimConnect<'static>>>,
    data_objects: Vec<Box<dyn SimDataObject>>,
    event_table: EventIdTable
}

impl SimWrapper {
    /// Create a new simwrapper and attempt to connect to the sim.
    /// 
    /// name - the name of the sim connection
    /// data_objects - the objects to fetch data from
    pub fn new(name: String, data_objects: Vec<Box<dyn SimDataObject>>) -> Result<Self, Box<dyn std::error::Error>> {
        let sim = match SimConnect::open(name.as_str(), | _sim, _recv | {} ) {
                Ok(sim) => sim,
                Err(e) => {
                    return Err(Box::new(e));
                }
            };

        Ok(SimWrapper { 
            sim_object: sim,
            data_objects: data_objects,
            event_table: EventIdTable::new()
        })
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
    pub fn write_event<T>(&mut self, name: String, value: sys::DWORD) -> Result<(), Box<dyn std::error::Error>> {
        let event_num = match self.event_table.get(name) {
            Some(num) => *num,
            None => return Err(Box::from(format!("event {} not in current table try registering it", name)))
        };

        self
            .sim_object
            .transmit_client_event(SIMCONNECT_OBJECT_ID_USER, event_num as sys::DWORD, value)?;

        Ok(())
    }
}

pub struct SimFreq {
    wrapper: SimWrapper
} 

impl SimFreq {
    /// Create a new object to comunicate with the sim regarding frequencies
    pub fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let data_objects: Vec<Box<dyn SimDataObject>> = vec![Box::new(FrequencyData{com1_active: 118.000, com1_standby:118.000})];
        let mut wrapper = SimWrapper::new("Frequency Communication".to_string(), data_objects)?;
        wrapper.register_event("COM_RADIO_SET_HZ".to_string())?;
        wrapper.register_event("COM_STBY_RADIO_SET_HZ".to_string())?;

        Ok(SimFreq { wrapper: wrapper })
    }

    /// Set a specific frequency
    pub fn set(&self, event)
}