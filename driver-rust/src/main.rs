use serialport::SerialPort;
use std::io::{Read};
use std::time::Duration;
use std::error::Error;
use customCANProtocol::{Packet, PacketHandler};


/// Open the serial port
/// 
fn open_serial_port(port_name: &str, baud_rate: u32) -> Result<Box<dyn SerialPort>, Box<dyn std::error::Error>> {
    let port = serialport::new(port_name, baud_rate)
        .timeout(Duration::from_millis(1000))
        .open()
        .map_err(|e| Box::new(e) as Box<dyn std::error::Error>)?;

    Ok(port)
}

struct FreqPacketHandler;

impl FreqPacketHandler {
    fn new() -> Self {
        FreqPacketHandler {}
    }
}

impl PacketHandler for FreqPacketHandler {
    fn handle_packet(&mut self, packet: &Packet) -> Result<(), Box<dyn Error>> {
        
        println!("Freq packet: {:?}", packet);
        
        let mut activefreqMHz: u16 = ((packet.payload[0] as u16) << 8) | (packet.payload[1] as u16);
        let mut activefreqKHz: u16 = ((packet.payload[2] as u16) << 8) | (packet.payload[3] as u16);

        println!("Active Freq: {}.{}", activefreqMHz, activefreqKHz);

        return Ok(());
    }

    fn get_id(&self) -> u8 {
        1
    }
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

    let mut freq_packet_handler = FreqPacketHandler::new();
    

    println!("Reading from serial port: {}", port_name);
    loop {
        match Packet::read_from_stream(&mut port) {
            Ok(packet) => {
                // Successfully read the packet, so we can print and use it
                println!("{:?}", packet);

                if packet.packet_ident == freq_packet_handler.get_id() {
                    freq_packet_handler.handle_packet(&packet).unwrap();
                }
        
                // You can now use 'packet' further in your code
                // For example:
                // process_packet(packet);
            },
            Err(e) => {
                // Handle the error if reading from the stream fails
                eprintln!("Error reading packet: {}", e);
            }
        }

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