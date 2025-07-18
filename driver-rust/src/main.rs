use std::{thread, time::Duration};

use serialport::SerialPort;

use custom_can_protocol::{Packet, PacketHandler};

mod sim_wrap;

mod sim_freq;

mod device_select;

use device_select::DeviceSelectHandler;

mod freq;

use freq::FreqHandler;


/// Open the serial port
///
fn open_serial_port(
    port_name: &str,
    baud_rate: u32,
) -> Result<Box<dyn SerialPort>, Box<dyn std::error::Error>> {
    let port = serialport::new(port_name, baud_rate)
        .timeout(Duration::from_millis(1))
        .open()
        .map_err(|e| Box::new(e) as Box<dyn std::error::Error>)?;

    Ok(port)
}

fn main() {
    let port_name = "com9";
    let baud_rate = 115200;

    let mut port = match open_serial_port(port_name, baud_rate) {
        Ok(port) => port,
        Err(e) => {
            eprintln!("Failed to open serial port: {}", e);
            return;
        }
    };

    let mut device_select_handler = DeviceSelectHandler::new();
    let mut freq_packet_handler = FreqHandler::new();

    println!("Reading from serial port: {}", port_name);

    loop {
        if let Some(mut packets) = freq_packet_handler.check_for_freq_updates() {
            println!("Sending {:?}", packets);
            for packet in packets.iter_mut() {
                packet.compile();
                packet.write_to_stream(&mut port);

            }
        }
        if port.bytes_to_read().unwrap() > 10 {
            match Packet::read_from_stream(&mut port) {
                Ok(packet) => {
                    if packet.packet_ident == freq_packet_handler.get_packet_id() {
                        freq_packet_handler.handle_packet(&packet).unwrap();
                    } else if packet.packet_ident == device_select_handler.get_packet_id() {
                        device_select_handler.handle_packet(&packet).unwrap();
                    }
                }
                Err(e) => {
                    eprintln!("Error reading packet: {:?}", e);
                }
            }
        }
    }
}
