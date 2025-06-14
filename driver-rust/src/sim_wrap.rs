/// A wrapper around the sim connect interface to allow easy reuse
/// 
/// Author: Jack Duignan (JackpDuignan@gmail.com)

use msfs::{sim_connect::{self, data_definition, Period, SimConnect}, sys::{SIMCONNECT_OBJECT_ID, SIMCONNECT_OBJECT_ID_USER}};

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
}

trait SimDataObject {
    /// Register a request with the simulator to get the data.
    /// 
    /// sim - the simulator connection
    /// 
    /// returns an error if it cannot register the request
    fn register_request(&self, sim: &mut Pin<Box<SimConnect<'static>>>, id: u32) -> Result<(), msfs::sim_connect::HResult>;
}


#[data_definition]
#[derive(Debug)]
struct FrequencyData {
    #[name = "COM STANDBY FREQUENCY:1"]
    #[unit = "MHz"]
    com1_standby_frequency: f64,
    #[name = "COM ACTIVE FREQUENCY:1"]
    #[unit = "MHz"]
    com1_active_frequency: f64,
}

impl SimDataObject for FrequencyData {
    fn register_request(&self, sim: &mut Pin<Box<SimConnect<'static>>>, id: u32) -> Result<(), msfs::sim_connect::HResult> {
        sim.request_data_on_sim_object::<FrequencyData>(id, SIMCONNECT_OBJECT_ID_USER, Period::SimFrame);

        Ok(())
    }
}


pub struct SimWrapper {
    sim_object: Pin<Box<sim_connect::SimConnect<'static>>>,
    // data: Vec<Box<dyn SimDataObject>>,
    event_table: EventIdTable
}

impl SimWrapper {
    /// Create a new simwrapper and attempt to connect to the sim.
    /// 
    /// name - the name of the sim connection
    pub fn new(name: String, ) -> Result<Self, Box<dyn std::error::Error>> {
        let sim = match SimConnect::open(name.as_str(), | _sim, _recv | {} ) {
                Ok(sim) => sim,
                Err(e) => {
                    return Err(Box::new(e));
                }
            };

        Ok(SimWrapper { 
            sim_object: sim,
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
    fn register_event(sim: &mut Pin<Box<SimConnect<'static>>>, event_name: String) -> Result<u32, Box<dyn std::error::Error>> {
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
    pub fn add_event(&mut self, name: String) -> Result<(), Box<dyn std::error::Error>> {
        match SimWrapper::register_event(&mut self.sim_object, name.clone()) {
            Ok(id) => {
                self.event_table.add(name, id);
            },
            Err(e) => {
                return Err(e);
            }
        };
            
        Ok(())
    }
}