use std::pin::Pin;
use std::error::Error;

use msfs::sys;

use msfs::sim_connect::SimConnect;

pub struct MSFSHandler<'a> {
    sim_connect: Pin<Box<SimConnect<'a>>>,
}

impl MSFSHandler<'_> {
    pub fn new() -> Self {
        let sim = match SimConnect::open("FLIGHT_SIM_SOFTWARE", |_sim, recv| {
            println!("WRITER: {:?}", recv);
        }) {
            Ok(sim) => sim,
            Err(e) => panic!("Error opening SimConnect: {:?}", e)
        };

        MSFSHandler {
            sim_connect: sim
        }
    }

    pub fn update_freq(&mut self, active_freq_mhz: u16, active_freq_khz: u16, standby_freq_mhz: u16, standby_freq_khz: u16) -> Result<(), Box<dyn Error>> {
        let event_com1_active_set: u32 = match self.sim_connect.map_client_event_to_sim_event("COM_RADIO_SET_HZ", false) {
            Ok(event_id) => {
                event_id
            }
            Err(e) => {
                eprintln!("Failed to map COM1 event: {:?}", e);
                return Err(e.into()); // Convert HResult to a more general error type if needed
            }
        };

        let event_com1_standby_set: u32 = match self.sim_connect.map_client_event_to_sim_event("COM_STBY_RADIO_SET_HZ", false) {
            Ok(event_id) => {
                event_id
            }
            Err(e) => {
                eprintln!("Failed to map COM1 event: {:?}", e);
                return Err(e.into()); // Convert HResult to a more general error type if needed
            }
        };
    
        let com1_active_freq: u32 = active_freq_mhz as u32 * 1000000 + active_freq_khz as u32 * 1000;
        let com1_standby_freq: u32 = standby_freq_mhz as u32 * 1000000 + standby_freq_khz as u32 * 1000;
    
        if let Err(e) = self.sim_connect.transmit_client_event(event_com1_active_set, event_com1_active_set, com1_active_freq as sys::DWORD) {
            eprintln!("Failed to transmit event: {:?}", e);
        }

        if let Err(e) = self.sim_connect.transmit_client_event(0, event_com1_standby_set, com1_standby_freq as sys::DWORD) {
            eprintln!("Failed to transmit event: {:?}", e);
        }

        Ok(())
    }
}