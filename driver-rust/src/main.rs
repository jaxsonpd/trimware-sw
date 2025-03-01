use serialport::SerialPort;
use std::collections::btree_map::Range;
use std::fs::read;
use std::io::{self, Read, Write};
use std::time::Duration;
use customCANProtocol::{Packet, PacketByteLocations};


/// Open the serial port
/// 
fn open_serial_port(port_name: &str, baud_rate: u32) -> Result<Box<dyn SerialPort>, Box<dyn std::error::Error>> {
    let port = serialport::new(port_name, baud_rate)
        .timeout(Duration::from_millis(1000))
        .open()
        .map_err(|e| Box::new(e) as Box<dyn std::error::Error>)?;

    Ok(port)
}

/// Read a custom can protocol packet from the serial port
/// 
/// # Parameters
/// - `port` the serial port to read from
/// 
/// # Returns
/// Return the packet as a buffer of u8s
fn read_packet(port: &mut Box<dyn SerialPort>) -> Result<Box<Vec<u8>>, Box<dyn std::error::Error>> {
    let mut buffer = vec![0u8; customCANProtocol::MAX_PACKET_LENGTH];
    let mut idx = 0;
    let mut read_buffer: Vec<u8> = vec![0; 1];

    loop {
        let num_bytes = port.read(&mut read_buffer)?;
        if num_bytes > 0 {
            let byte = read_buffer[0];

            if byte == customCANProtocol::PACKET_START_BYTE {
                buffer[idx] = byte;
                idx += 1;
                break;
            }
        }
    }
    
    loop {
        let num_bytes = port.read(&mut read_buffer)?;
        if num_bytes > 0 {
            buffer[idx] = read_buffer[0];
            idx += 1; 
            if read_buffer[0] == customCANProtocol::PACKET_START_BYTE {
                break;       
            }
        }
    }
    
    print!("Got packet: ");
    for &x in &buffer[..idx] {
        print!("{}-", x);
    }
    print!("\n");

    Ok(Box::new(buffer))
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
    

    println!("Reading from serial port: {}", port_name);
    loop {
        let Packet = Packet::read_from_stream(&mut port);
        // let packet: Vec<u8>;
        // if let Ok(data) = read_packet(&mut port) {
        //     // `data` is a `Box<Vec<u8>>`
        //     packet= *data;
        // } else {
        //     eprintln!("Error reading packet");
        //     // Handle the error as appropriate
        // }
        // let read_packet = customCANProtocol::Packet::new(packet[1], packet[1..]);
        // read_packet.validate();
    }
}