use std::pin::Pin;
use std::error::Error;

use msfs::sys;

use msfs::sim_connect::SimConnect;

pub enum FreqOptions {
    Active,
    Standby
}

pub enum MSFSRadioDevices {
    COM1,
    COM2,
    COM3,
    NAV1,
    DME,
    ADF,
    XPDR
}


pub struct MSFSCommunicator<'a> {
    sim_connect: Pin<Box<SimConnect<'a>>>,
    event_com1_active_set: u32,
    event_com1_standby_set: u32,
    event_com2_active_set: u32,
    event_com2_standby_set: u32,
    event_nav1_active_set: u32,
    event_nav1_standby_set: u32,
    event_nav2_active_set: u32,
    event_nav2_standby_set: u32,
}

impl MSFSCommunicator<'_> {
    /// Add an event to the list
    /// 
    /// # Arguments
    /// - sim the sim to affect
    /// - event_name the text string event name
    /// 
    /// # Returns
    /// - The instance specific event number
    fn add_event(sim: Pin<Box<SimConnect<'_>>>, event_name: &str) -> u32 {
        let event_id: u32 = match self.sim_connect.map_client_event_to_sim_event(event_name, false) {
            Ok(event_id) => {
                event_id
            }
            Err(e) => {
                eprintln!("Failed to map COM1 event: {:?}", e);
                return Err(e.into()); // Convert HResult to a more general error type if needed
            }
        };
        
        event_id
    }

    pub fn new() -> Self {
        let sim = match SimConnect::open("FLIGHT_SIM_SOFTWARE", |_sim, recv| {
            println!("WRITER: {:?}", recv);
        }) {
            Ok(sim) => sim,
            Err(e) => panic!("Error opening SimConnect: {:?}", e)
        };

        // Setup events
        let event_com1_active_set:u32 = MSFSCommunicator::add_event(sim, "COM_RADIO_SET_HZ");
        let event_com1_standby_set: u32 = MSFSCommunicator::add_event(sim, "COM_STBY_RADIO_SET_HZ");
        let event_com2_active_set:u32 = MSFSCommunicator::add_event(sim, "COM2_RADIO_SET_HZ");
        let event_com2_standby_set: u32 = MSFSCommunicator::add_event(sim, "COM2_STBY_RADIO_SET_HZ");
        let event_nav1_active_set: u32 = MSFSCommunicator::add_event(sim, "NAV1_RADIO_SET_HZ");
        let event_nav1_standby_set: u32 = MSFSCommunicator::add_event(sim, "NAV1_STBY_SET_HZ");
        let event_nav2_active_set: u32 = MSFSCommunicator::add_event(sim, "NAV2_RADIO_SET_HZ");
        let event_nav2_standby_set: u32 = MSFSCommunicator::add_event(sim, "NAV2_STBY_SET_HZ");

        MSFSCommunicator {
            sim_connect: sim,
            event_com1_active_set,
            event_com1_standby_set,
            event_com2_active_set,
            event_com2_standby_set,
            event_nav1_active_set,
            event_nav1_standby_set,
            event_nav2_active_set,
            event_nav2_standby_set,
        }
    }

    /// Update the frequency value
    /// 
    /// # Arguments
    /// - device the device to set COM1 etc.
    /// - option the active or standby frequency
    /// - freq the frequency to set in Hz
    /// 
    /// # Returns
    /// error if cannot be set
    pub fn update_freq(&mut self, device: MSFSRadioDevices, option: FreqOptions, freq: u64) -> Result<(), Box<dyn Error>> {
        let mut event: u32;

        match (device, option) {
            (MSFSRadioDevices::Com1, FreqOptions::Active) => event = self.event_com1_active_set,
            (MSFSRadioDevices::Com1, FreqOptions::Standby) => event = self.event_com1_standby_set,
            (MSFSRadioDevices::Com2, FreqOptions::Active) => event = self.event_com2_active_set,
            (MSFSRadioDevices::Com2, FreqOptions::Standby) => event = self.event_com2_standby_set,
            (MSFSRadioDevices::Nav1, FreqOptions::Active) => event = self.event_nav1_active_set,
            (MSFSRadioDevices::Nav1, FreqOptions::Standby) => event = self.event_nav1_standby_set,
            (MSFSRadioDevices::Nav2, FreqOptions::Active) => event = self.event_nav2_active_set,
            (MSFSRadioDevices::Nav2, FreqOptions::Standby) => event = self.event_nav2_standby_set,
            _ => return Err(Box::new(MSFSCommunicatorError::InvalidDevice)),
        }

        if let Err(e) = self.sim_connect.transmit_client_event(0, event, freq as sys::DWORD) {
            eprintln!("Failed to transmit event: {:?}", e);
        }

        Ok(())
    }
}