use std::error::Error;

use crate::msfs_connect::MSFSCommunicator

use customCANProtocol::{Packet, PacketHandler};

pub struct FreqHandler<'a> {
    msfs_connection: MSFSCommunicator<'a>,
}

impl FreqHandler<'_> {
    pub fn new(msfs_connection: MSFSCommunicator) -> Self {
        FreqHandler {
            msfs_connection
        }
    }
}

impl PacketHandler for FreqHandler<'_> {
    fn handle_packet(&mut self, packet: &Packet) -> Result<(), Box<dyn Error>> {
        
        println!("Freq packet: {:?}", packet);
        
        let standby_freq_mhz: u16 = ((packet.payload[0] as u16) << 8) | (packet.payload[1] as u16);
        let standby_freq_khz: u16 = ((packet.payload[2] as u16) << 8) | (packet.payload[3] as u16);

        let active_freq_mhz: u16 = ((packet.payload[4] as u16) << 8) | (packet.payload[5] as u16);
        let active_freq_khz: u16 = ((packet.payload[6] as u16) << 8) | (packet.payload[7] as u16);

        
        self.msfs_connection.update_freq(, option, freq)

        return Ok(());
    }

    fn get_id(&self) -> u8 {
        1
    }
}