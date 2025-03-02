use std::time::Duration;
use std::error::Error;

use msfs::sys;
use serialport::SerialPort;

use msfs::sim_connect::SimConnect;

use customCANProtocol::{Packet, PacketHandler};

mod msfs_handler;

use msfs_handler::MSFSHandler;

mod freq;

use freq::FreqPacketHandler;

/// Open the serial port
/// 
fn open_serial_port(port_name: &str, baud_rate: u32) -> Result<Box<dyn SerialPort>, Box<dyn std::error::Error>> {
    let port = serialport::new(port_name, baud_rate)
        .timeout(Duration::from_millis(1000))
        .open()
        .map_err(|e| Box::new(e) as Box<dyn std::error::Error>)?;

    Ok(port)
}

fn open_msfs2020_connection() -> Result<(), Box<dyn Error>> {
    let mut sim = SimConnect::open("FLIGHT_SIM_SOFTWARE", |_sim, recv| {
        println!("WRITER: {:?}", recv);
    })?;

    let event_com1_set: u32 = match sim.map_client_event_to_sim_event("COM_RADIO_SET_HZ", false) {
        Ok(event_id) => {
            println!("Successfully mapped COM1 event. Event ID: {}", event_id);
            event_id
        }
        Err(e) => {
            eprintln!("Failed to map COM1 event: {:?}", e);
            return Err(e.into()); // Convert HResult to a more general error type if needed
        }
    };

    let com1_freq = 128_450_000;

    if let Err(e) = sim.transmit_client_event(event_com1_set, event_com1_set, com1_freq as sys::DWORD) {
        eprintln!("Failed to transmit event: {:?}", e);
    } else {
        println!("COM1 frequency set to 123.45 MHz.");
    }
    Ok(())
}

fn set_msfs2020_freq(active_freq_mhz: u16, active_freq_khz: u16, standby_freq_mhz: u16, standby_freq_khz: u16) -> Result<(), Box<dyn Error>> {
    println!("Setting active freq to {} MHz, {} KHz", active_freq_mhz, active_freq_khz);
    println!("Setting standby freq to {} MHz, {} KHz", standby_freq_mhz, standby_freq_khz);

    let mut msfs_handler = MSFSHandler::new();
    let _ = msfs_handler.update_freq(active_freq_mhz, active_freq_khz, standby_freq_mhz, standby_freq_khz);
    

    Ok(())
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

    let mut freq_packet_handler = FreqPacketHandler::new(set_msfs2020_freq);  

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