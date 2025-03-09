use std::pin::Pin;

use msfs::sys::{self, BCD16};

use msfs::sim_connect::{SimConnectRecv, SimConnect, data_definition, SIMCONNECT_OBJECT_ID_USER, Period};

pub enum MSFSFreqOptions {
    Active,
    Standby,
}

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum MSFSRadioDevices {
    COM1,
    COM2,
    NAV1,
    NAV2,
    DME,
    ADF,
    XPDR,
}

#[derive(Clone, Copy, Debug)]
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
    event_xpndr_set: u32
}

#[data_definition]
#[derive(Debug)]
struct FrequencyData {
    #[name = "COM STANDBY FREQUENCY:1"]
    #[unit = "Frequency"]
    com1_standby_frequency: f64,
    #[name = "COM ACTIVE FREQUENCY:1"]
    #[unit = "Frequency"]
    com1_active_frequency: f64,
    #[name = "NAV STANDBY FREQUENCY:1"]
    #[unit = "Frequency"]
    nav1_standby_frequency: f64,
    #[name = "NAV ACTIVE FREQUENCY:1"]
    #[unit = "Frequency"]
    nav1_active_frequency: f64,
    #[name = "NAV STANDBY FREQUENCY:2"]
    #[unit = "Frequency"]
    nav2_standby_frequency: f64,
    #[name = "NAV ACTIVE FREQUENCY:2"]
    #[unit = "Frequency"]
    nav2_active_frequency: f64,
    #[name = "COM STANDBY FREQUENCY:2"]
    #[unit = "Frequency"]
    com2_standby_frequency: f64,
    #[name = "COM ACTIVE FREQUENCY:2"]
    #[unit = "Frequency"]
    com2_active_frequency: f64,
}

fn u64_to_bcd16(value: u64) -> u64 {
    let mut bcd: u64 = 0;
    let mut shift = 0;
    let mut num = value;

    while num > 0 {
        let digit = num % 10;
        bcd |= (digit as u64) << shift;
        num /= 10;
        shift += 4;
    }

    bcd
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
        let mut sim = match SimConnect::open("FLIGHT_SIM_SOFTWARE", |sim, recv| match recv {
            SimConnectRecv::SimObjectData(event) => match event.dwRequestID {
                0 => {
                    println!("{:?}", event.into::<FrequencyData>(sim).unwrap());
                }
                _ => {}
            },
            _ => println!("{:?}", recv)
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
        let event_xpndr_set: u32 = MSFSComms::add_event(&mut sim, "XPNDR_SET");
        
        let result: Result<(), msfs::sim_connect::HResult> = sim.request_data_on_sim_object::<FrequencyData>(0, SIMCONNECT_OBJECT_ID_USER, Period::SimFrame);
        print!("result {:?}", result);

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
            event_xpndr_set,
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
            (MSFSRadioDevices::COM1, MSFSFreqOptions::Standby) => event = self.event_com1_standby_set,
            (MSFSRadioDevices::COM2, MSFSFreqOptions::Active) => event = self.event_com2_active_set,
            (MSFSRadioDevices::COM2, MSFSFreqOptions::Standby) => event = self.event_com2_standby_set,
            (MSFSRadioDevices::NAV1, MSFSFreqOptions::Active) => event = self.event_nav1_active_set,
            (MSFSRadioDevices::NAV1, MSFSFreqOptions::Standby) => event = self.event_nav1_standby_set,
            (MSFSRadioDevices::NAV2, MSFSFreqOptions::Active) => event = self.event_nav2_active_set,
            (MSFSRadioDevices::NAV2, MSFSFreqOptions::Standby) => event = self.event_nav2_standby_set,
            (MSFSRadioDevices::XPDR, MSFSFreqOptions::Active) => event = self.event_xpndr_set,
            _ => return Err(MSFSCommunicatorError::InvalidDevice),
        }
        let mut freq = freq;

        if event == self.event_xpndr_set {
            // Convert to BCD16
            freq = u64_to_bcd16(freq);
            println!("Setting XPDR to {}", freq);
        }

        if let Err(e) = self
            .sim_connect
            .transmit_client_event(SIMCONNECT_OBJECT_ID_USER, event, freq as sys::DWORD)
        {
            eprintln!("Failed to transmit event: {:?}", e);
        }

        Ok(())
    }

    /// Get a new frequency for the given device and option.
    /// 
    /// # Argument
    /// - device the device to get the frequency from
    /// - option the active or standby frequency
    /// 
    /// # Returns
    /// - the frequency in hz
    pub fn get_frequency(&mut self, device: &MSFSRadioDevices, option: &MSFSFreqOptions) -> Result<u32, MSFSCommunicatorError> {
        self.sim_connect.call_dispatch();
        Ok(1)
    }
}
