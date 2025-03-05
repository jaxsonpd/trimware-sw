use std::pin::Pin;

use msfs::sys;

use msfs::sim_connect::SimConnect;

pub enum MSFSFreqOptions {
    Active,
    Standby,
}

#[derive(Clone, Copy, Debug)]
pub enum MSFSRadioDevices {
    COM1,
    COM2,
    NAV1,
    NAV2,
    DME,
    ADF,
    XPDR,
}

#[derive(Clone, Copy)]
pub enum MSFSCommunicatorError {
    InvalidDevice,
}

pub struct MSFSComms<'a> {
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

impl MSFSComms<'_> {
    /// Add an event to the list
    ///
    /// # Arguments
    /// - sim the sim to affect
    /// - event_name the text string event name
    ///
    /// # Returns
    /// - The instance specific event number
    fn add_event(sim: &mut Pin<Box<SimConnect>>, event_name: &str) -> u32 {
        let event_id: u32 = match sim.map_client_event_to_sim_event(event_name, false) {
            Ok(id) => id,
            Err(err) => {
                eprintln!("Failed to map event: {:?}", err);
                return 0;
            }
        };

        event_id
    }

    pub fn new() -> Self {
        let mut sim = match SimConnect::open("FLIGHT_SIM_SOFTWARE", |_sim, recv| {
            println!("WRITER: {:?}", recv);
        }) {
            Ok(sim) => sim,
            Err(e) => panic!("Error opening SimConnect: {:?}", e),
        };

        // Setup events
        let event_com1_active_set: u32 = MSFSComms::add_event(&mut sim, "COM_RADIO_SET_HZ");
        let event_com1_standby_set: u32 = MSFSComms::add_event(&mut sim, "COM_STBY_RADIO_SET_HZ");
        let event_com2_active_set: u32 = MSFSComms::add_event(&mut sim, "COM2_RADIO_SET_HZ");
        let event_com2_standby_set: u32 = MSFSComms::add_event(&mut sim, "COM2_STBY_RADIO_SET_HZ");
        let event_nav1_active_set: u32 = MSFSComms::add_event(&mut sim, "NAV1_RADIO_SET_HZ");
        let event_nav1_standby_set: u32 = MSFSComms::add_event(&mut sim, "NAV1_STBY_SET_HZ");
        let event_nav2_active_set: u32 = MSFSComms::add_event(&mut sim, "NAV2_RADIO_SET_HZ");
        let event_nav2_standby_set: u32 = MSFSComms::add_event(&mut sim, "NAV2_STBY_SET_HZ");

        MSFSComms {
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
    pub fn update_freq(
        &mut self,
        device: &MSFSRadioDevices,
        option: &MSFSFreqOptions,
        freq: u64,
    ) -> Result<(), MSFSCommunicatorError> {
        let event: u32;

        match (device, option) {
            (MSFSRadioDevices::COM1, MSFSFreqOptions::Active) => event = self.event_com1_active_set,
            (MSFSRadioDevices::COM1, MSFSFreqOptions::Standby) => {
                event = self.event_com1_standby_set
            }
            (MSFSRadioDevices::COM2, MSFSFreqOptions::Active) => event = self.event_com2_active_set,
            (MSFSRadioDevices::COM2, MSFSFreqOptions::Standby) => {
                event = self.event_com2_standby_set
            }
            (MSFSRadioDevices::NAV1, MSFSFreqOptions::Active) => event = self.event_nav1_active_set,
            (MSFSRadioDevices::NAV1, MSFSFreqOptions::Standby) => {
                event = self.event_nav1_standby_set
            }
            (MSFSRadioDevices::NAV2, MSFSFreqOptions::Active) => event = self.event_nav2_active_set,
            (MSFSRadioDevices::NAV2, MSFSFreqOptions::Standby) => {
                event = self.event_nav2_standby_set
            }
            _ => return Err(MSFSCommunicatorError::InvalidDevice),
        }

        if let Err(e) = self
            .sim_connect
            .transmit_client_event(0, event, freq as sys::DWORD)
        {
            eprintln!("Failed to transmit event: {:?}", e);
        }

        Ok(())
    }
}