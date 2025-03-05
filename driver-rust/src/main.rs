use std::time::Duration;
use std::error::Error;

use serialport::SerialPort;

use customCANProtocol::{Packet, PacketHandler};

mod msfs_connect;

use msfs_connect::MSFSCommunicator;

mod device_select;

use device_select::DeviceSelectHandler;

mod freq;

use freq::FreqHandler;

/// Open the serial port
/// 
fn open_serial_port(port_name: &str, baud_rate: u32) -> Result<Box<dyn SerialPort>, Box<dyn std::error::Error>> {
    let port = serialport::new(port_name, baud_rate)
        .timeout(Duration::from_millis(1000))
        .open()
        .map_err(|e| Box::new(e) as Box<dyn std::error::Error>)?;

    Ok(port)
}

fn main() {
    let port_name = "com3";
    let baud_rate = 115200;

    let mut port = match open_serial_port(port_name, baud_rate) {
        Ok(port) => port,
        Err(e) => {
            eprintln!("Failed to open serial port: {}", e);
            return;
        }
    };

    let mut msfs_handler = MSFSCommunicator::new();

    let mut device_select_handler = DeviceSelectHandler::new(msfs_handler);

    let mut freq_packet_handler = FreqHandler::new(msfs_handler);  

    println!("Reading from serial port: {}", port_name);

    loop {
        match Packet::read_from_stream(&mut port) {
            Ok(packet) => {
                println!("{:?}", packet);

                if packet.packet_ident == freq_packet_handler.get_id() {
                    freq_packet_handler.handle_packet(&packet).unwrap();
                } else if packet.packet_ident == device_select_handler.get_id() {
                    device_select_handler.handle_packet(&packet).unwrap();
                }
            },
            Err(e) => {
                eprintln!("Error reading packet: {}", e);
            }
        }
    }
}