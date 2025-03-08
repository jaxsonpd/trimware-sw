use std::error::Error;
use std::cell::RefCell;

use crate::{msfs_connect::MSFSComms, msfs_connect::MSFSFreqOptions, msfs_connect::MSFSRadioDevices};
use crate::device_select::DeviceSelectHandler;

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
        self.msfs_connection.get_frequency(&MSFSRadioDevices::COM1, &MSFSFreqOptions::Active);

        
        let standby_freq_mhz: u16 = ((packet.payload[0] as u16) << 8) | (packet.payload[1] as u16);
        let standby_freq_khz: u16 = ((packet.payload[2] as u16) << 8) | (packet.payload[3] as u16);

        let active_freq_mhz: u16 = ((packet.payload[4] as u16) << 8) | (packet.payload[5] as u16);
        let active_freq_khz: u16 = ((packet.payload[6] as u16) << 8) | (packet.payload[7] as u16);

        let standby_freq = ((standby_freq_mhz as u32) * 1e6 as u32
                        + (standby_freq_khz as u32) * 1e3 as u32) as u64;
        
        let active_freq  = ((active_freq_mhz as u32) * 1e6 as u32
                        + (active_freq_khz as u32) * 1e3 as u32) as u64;

        let selected_device = self.device_select_handler.borrow().get_selected_device();
        
        println!("Set frequency {} {} to device {:?}", active_freq/1000, standby_freq/1000, selected_device);

        match self.msfs_connection.update_freq(&selected_device, &MSFSFreqOptions::Active, active_freq) {
            Ok(_e) => {
            }
            Err(e) => {
                println!("Error setting frequency: {:?}", e);
            }
        };
        match self.msfs_connection.update_freq(&selected_device, &MSFSFreqOptions::Standby, standby_freq){
            Ok(_e) => {
            }
            Err(e) => {
                println!("Error setting frequency: {:?}", e);
            }
        };

        return Ok(());
    }

    fn get_packet_id(&self) -> u8 {
        1
    }
}