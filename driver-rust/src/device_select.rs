use std::error::Error;

use customCANProtocol::{Packet, PacketHandler};

use crate::msfs_connect::MSFSRadioDevices;

pub struct DeviceSelectHandler {
    selected_device: MSFSRadioDevices,
}

impl DeviceSelectHandler {
    pub fn new() -> Self {
        DeviceSelectHandler {
            selected_device: MSFSRadioDevices::COM1,
        }
    }

    pub fn get_selected_device(&self) -> MSFSRadioDevices {
        self.selected_device
    }
}

impl PacketHandler for DeviceSelectHandler {
    fn handle_packet(&mut self, packet: &Packet) -> Result<(), Box<dyn Error>> {
        let packet_device = packet.payload[0];

        match packet_device {
            0 => self.selected_device = MSFSRadioDevices::COM1,
            1 => self.selected_device = MSFSRadioDevices::COM2,
            2 => self.selected_device = MSFSRadioDevices::NAV1,
            3 => self.selected_device = MSFSRadioDevices::NAV2,
            _ => println!("Unknown device: {}", packet_device),
        }

        Ok(())
    }

    fn get_id(&self) -> u8 {
        4
    }
}
