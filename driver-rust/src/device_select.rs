use std::{error::Error};

use custom_can_protocol::{Packet, PacketHandler};

use crate::sim_freq::RadioDevices;

/// Handle the selection of radio devices
pub struct DeviceSelectHandler {
    selected_device: RadioDevices,
}

/// Convert from a device select packet number to radio devices
pub fn convert_to_device(number: u8) -> RadioDevices {
    match number {
        0 => RadioDevices::COM1,
        1 => RadioDevices::COM2,
        2 => RadioDevices::NAV1,
        3 => RadioDevices::NAV2,
        6 => RadioDevices::XPDR,
        _ => RadioDevices::COM1,
    }
}


impl DeviceSelectHandler {
    pub fn new() -> Self {
        DeviceSelectHandler {
            selected_device: RadioDevices::COM1,
        }
    }

    pub fn get_selected_device(&self) -> RadioDevices {
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
            0 => self.selected_device = RadioDevices::COM1,
            1 => self.selected_device = RadioDevices::COM2,
            2 => self.selected_device = RadioDevices::NAV1,
            3 => self.selected_device = RadioDevices::NAV2,
            6 => self.selected_device = RadioDevices::XPDR,
            _ => println!("Unknown device: {}", packet_device),
        }

        println!("Selected device: {:?}", packet_device);

        Ok(())
    }

    fn get_packet_id(&self) -> u8 {
        4
    }
}
