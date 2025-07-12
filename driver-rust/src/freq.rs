use std::error::Error;
use std::cell::RefCell;

use crate::sim_wrap::{FrequencyName, SimFreq};
use crate::device_select::{DeviceSelectHandler, convert_to_device, RadioDevices};

use custom_can_protocol::{Packet, PacketHandler};

pub struct FreqHandler<'a> {
    sim_frequency_connection: SimFreq,
    device_select_handler: &'a RefCell<DeviceSelectHandler>,
}

impl<'a> FreqHandler<'a> {
    pub fn new(device_select_handler: &'a RefCell<DeviceSelectHandler>) -> Self {
        
        let sim_frequency_connection = SimFreq::new()
            .expect("Failed to connect to simulator try running the program again.");
        
        FreqHandler {
            sim_frequency_connection: sim_frequency_connection,
            device_select_handler: device_select_handler
        }
    }

    pub fn get_freq(&mut self) -> Result<u64, Box<dyn Error>> {
        Ok(1)
    }
}

impl PacketHandler for FreqHandler<'_> {
    fn handle_packet(&mut self, packet: &Packet) -> Result<(), Box<dyn Error>> {
        let radio_type = convert_to_device(packet.payload[0]);

        let mut standby_freq: u32 = ((packet.payload[1] as u32) << 24)
                                + ((packet.payload[2] as u32) << 16)
                                + ((packet.payload[3] as u32) << 8)
                                + ((packet.payload[4] as u32) << 0);
        
        let mut active_freq: u32 = ((packet.payload[5] as u32) << 24)
                                + ((packet.payload[6] as u32) << 16)
                                + ((packet.payload[7] as u32) << 8)
                                + ((packet.payload[8] as u32) << 0);

        if radio_type == RadioDevices::XPDR {
            let xpdr_value = active_freq;
            println!("XPDR value set {}", xpdr_value);
        } else {
            active_freq *= 1000;
            standby_freq *= 1000;
            println!("Set frequency Active: {}, Standby: {} to device {:?}", active_freq/1000, standby_freq/1000, radio_type);

        }

        let active_name: FrequencyName;
        let standby_name: FrequencyName;

        match radio_type {
            RadioDevices::COM1 => {
                active_name = FrequencyName::Com1Active;
                standby_name = FrequencyName::Com1Standby;
            }
            _ => {
                active_name = FrequencyName::Com1Active;
                standby_name = FrequencyName::Com1Standby;
            }
        }
            

        match self.sim_frequency_connection.set(active_name, active_freq) {
            Ok(_e) => {
            }
            Err(e) => {
                println!("Error setting active frequency: {:?}", e);
            }
        };
        match self.sim_frequency_connection.set(standby_name, standby_freq){
            Ok(_e) => {
            }
            Err(e) => {
                println!("Error setting standby frequency: {:?}", e);
            }
        };


        Ok(())
    }

    fn get_packet_id(&self) -> u8 {
        1
    }
}