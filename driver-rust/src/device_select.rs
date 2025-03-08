use std::{error::Error, vec};

use custom_can_protocol::{Packet, PacketHandler};

use crate::msfs_connect::MSFSRadioDevices;

/// Handle the selection of radio devices
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

    pub fn compose_request_device_packet(&self) -> Packet {
        Packet::new(self.get_packet_id(), Vec::new(), Vec::new())
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

        println!("Selected device: {:?}", self.selected_device);

        Ok(())
    }

    fn get_packet_id(&self) -> u8 {
        4
    }
}
