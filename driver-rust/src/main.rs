use std::{error::Error, thread, time::Duration};

use serialport::{SerialPort, SerialPortBuilder, SerialPortType};

use custom_can_protocol::{Packet, PacketHandler};

mod sim_wrap;

mod sim_freq;

mod device_select;

use device_select::DeviceSelectHandler;

mod freq;

use freq::FreqHandler;

/// Find available devices that could be interacted with
/// 
/// returns a vector of port name strings that match the give pids
fn get_available_ports(accepted_vid_pid: Vec<(u16, u16)>) -> Option<Vec<String>> {
    let ports = match serialport::available_ports() {
        Ok(ports) => ports,
        Err(_) => return None,
    };

    let mut available_ports: Vec<String> = Vec::new();

    for port in ports {
        for (vid, pid) in &accepted_vid_pid {
            match port.port_type {
                SerialPortType::UsbPort(ref usb_info) => {
                    if (usb_info.vid == *vid) && (usb_info.pid == *pid) {
                        available_ports.push(port.port_name.clone());
                    }   
                }
                _ => {break}
            }
        }
    }

    if available_ports.len() > 0 {
        return Some(available_ports);
    }

    return None
}


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
    let baud_rate = 115200;
    let accepted_vid_pid = vec![(6790, 29987), (0x10C4, 0xEA60)];
    let ports = match get_available_ports(accepted_vid_pid) {
        Some(ports) => {
            ports
        }
        None => {
            eprintln!("No available devices");
            return;
        }
    };

    let mut port = match open_serial_port(&ports[0], baud_rate) {
        Ok(port) => port,
        Err(e) => {
            eprintln!("Failed to open serial port: {}", e);
            return;
        }
    };

    let mut device_select_handler = DeviceSelectHandler::new();
    let mut freq_packet_handler = FreqHandler::new();

    println!("Reading from serial port: {}", &ports[0]);

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
