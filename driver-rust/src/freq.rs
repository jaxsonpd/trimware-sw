use std::error::Error;

use crate::sim_wrap::{FrequencyName, SimFreq};
use crate::device_select::{convert_to_device, RadioDevices};

use custom_can_protocol::{Packet, PacketHandler};

pub struct FreqHandler {
    sim_frequency_connection: SimFreq,
}

impl FreqHandler {
    pub fn new() -> Self {
        
        let sim_frequency_connection = SimFreq::new()
            .expect("Failed to connect to simulator try running the program again.");
        
        FreqHandler {
            sim_frequency_connection: sim_frequency_connection
        }
    }

    /// Check for frequency updates from the simulator
    /// 
    /// # Returns
    /// a vector of packets to send with the updated frequency data
    pub fn check_for_freq_updates(&mut self) -> Option<Vec<Packet>> {
        if let Some(data) = self.sim_frequency_connection.get_freq_update() {
            return None;
        }

        None
    }
}

fn u32_to_bcd16(value: u32) -> u32 {
    let mut bcd: u32 = 0;
    let mut shift = 0;
    let mut num = value;

    while num > 0 {
        let digit = num % 10;
        bcd |= (digit as u32) << shift;
        num /= 10;
        shift += 4;
    }

    bcd
}

impl PacketHandler for FreqHandler {
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
            RadioDevices::COM2 => {
                active_name = FrequencyName::Com2Active;
                standby_name = FrequencyName::Com2Standby;
            }
            RadioDevices::NAV1 => {
                active_name = FrequencyName::Nav1Active;
                standby_name = FrequencyName::Nav1Standby;
            }
            RadioDevices::NAV2 => {
                active_name = FrequencyName::Nav2Active;
                standby_name = FrequencyName::Nav2Standby;
            }
            RadioDevices::XPDR => {
                let xpdr_value = u32_to_bcd16(active_freq);
                self.sim_frequency_connection.set(FrequencyName::XPDR, xpdr_value)?;
                return Ok(());
            }
            _ => {
                return Ok(());
            }
        }
            

        self.sim_frequency_connection.set(active_name, active_freq)?;
        self.sim_frequency_connection.set(standby_name, standby_freq)?;

        Ok(())
    }

    fn get_packet_id(&self) -> u8 {
        1
    }
}