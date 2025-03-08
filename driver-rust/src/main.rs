use std::time::Duration;

use std::cell::RefCell;

use serialport::SerialPort;

use custom_can_protocol::{Packet, PacketHandler};

mod msfs_connect;

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

    let device_select_handler = RefCell::new(DeviceSelectHandler::new());
    let request = device_select_handler
        .borrow_mut()
        .compose_request_device_packet();
    let _ = request.write_to_stream(&mut port);

    let mut freq_packet_handler = FreqHandler::new(&device_select_handler);

    println!("Reading from serial port: {}", port_name);

    loop {
        match Packet::read_from_stream(&mut port) {
            Ok(packet) => {
                println!("{:?}", packet);
                if packet.packet_ident == freq_packet_handler.get_packet_id() {
                    freq_packet_handler.handle_packet(&packet).unwrap();
                } else if packet.packet_ident == device_select_handler.borrow().get_packet_id() {
                    device_select_handler
                        .borrow_mut()
                        .handle_packet(&packet)
                        .unwrap();
                }
            }
            Err(e) => {
                eprintln!("Error reading packet: {:?}", e);
            }
        }

        // let mut device_get_packet = device_select_handler.borrow().compose_request_device_packet();
        // device_get_packet.write_to_stream(&mut port);

        // std::thread::sleep(std::time::Duration::from_millis(100));
    }
}
