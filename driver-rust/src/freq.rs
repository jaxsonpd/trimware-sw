use std::error::Error;
use std::cell::RefCell;

use crate::{msfs_connect::MSFSComms, msfs_connect::MSFSFreqOptions, msfs_connect::MSFSRadioDevices};
use crate::device_select::{DeviceSelectHandler, convert_to_device};

use custom_can_protocol::{Packet, PacketHandler};

pub struct FreqHandler<'a> {
    msfs_connection: MSFSComms<'a>,
    device_select_handler: &'a RefCell<DeviceSelectHandler>,
}

impl<'a> FreqHandler<'a> {
    pub fn new(device_select_handler: &'a RefCell<DeviceSelectHandler>) -> Self {
        let mut msfs_connection = MSFSComms::new();
        
        FreqHandler {
            msfs_connection: msfs_connection,
            device_select_handler: device_select_handler
        }
    }
}

impl PacketHandler for FreqHandler<'_> {
    fn handle_packet(&mut self, packet: &Packet) -> Result<(), Box<dyn Error>> {
        let radio_type = convert_to_device(packet.payload[0]);

        let mut standby_freq: u64 = ((packet.payload[1] as u64) << 24)
                                + ((packet.payload[2] as u64) << 16)
                                + ((packet.payload[3] as u64) << 8)
                                + ((packet.payload[4] as u64) << 0);
        
        let mut active_freq: u64 = ((packet.payload[5] as u64) << 24)
                                + ((packet.payload[6] as u64) << 16)
                                + ((packet.payload[7] as u64) << 8)
                                + ((packet.payload[8] as u64) << 0);

        if radio_type == MSFSRadioDevices::XPDR {
            let xpdr_value = active_freq;
            println!("XPDR value set {}", xpdr_value);
        } else {
            active_freq *= 1000;
            standby_freq *= 1000;
            println!("Set frequency Active: {}, Standby: {} to device {:?}", active_freq/1000, standby_freq/1000, radio_type);

        }
            

        match self.msfs_connection.update_freq(&radio_type, &MSFSFreqOptions::Active, active_freq) {
            Ok(_e) => {
            }
            Err(e) => {
                println!("Error setting active frequency: {:?}", e);
            }
        };
        match self.msfs_connection.update_freq(&radio_type, &MSFSFreqOptions::Standby, standby_freq){
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