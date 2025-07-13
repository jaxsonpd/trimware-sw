/// A sim connection for use with frequency data
/// 
/// Author: Jack Duignan (JackpDuignan@gmail.com)

use std::{any::Any, collections::HashMap};

use msfs::{sim_connect::data_definition, sys::{SIMCONNECT_RECV_SIMOBJECT_DATA}};

use crate::sim_wrap::{SimDataObject, SimWrapper};

#[data_definition]
#[derive(Debug, Clone)]
struct Com1Data {
    #[name = "COM ACTIVE FREQUENCY:1"]
    #[unit = "MHz"]
    active: f64,
    #[name = "COM STANDBY FREQUENCY:1"]
    #[unit = "MHz"]
    standby: f64,
}

impl SimDataObject for Com1Data {
    fn get_event_data(
        &self,
        sim: &mut msfs::sim_connect::SimConnect<'_>,
        event: &SIMCONNECT_RECV_SIMOBJECT_DATA
    ) -> Option<Box<dyn SimDataObject>> {
        event
            .into::<Com1Data>(sim)
            .map(|data| Box::new(data.clone()) as Box<dyn SimDataObject>)
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}

#[data_definition]
#[derive(Debug, Clone)]
struct Com2Data {
    #[name = "COM ACTIVE FREQUENCY:2"]
    #[unit = "MHz"]
    active: f64,
    #[name = "COM STANDBY FREQUENCY:2"]
    #[unit = "MHz"]
    standby: f64,
}

impl SimDataObject for Com2Data {
    fn get_event_data(
        &self,
        sim: &mut msfs::sim_connect::SimConnect<'_>,
        event: &SIMCONNECT_RECV_SIMOBJECT_DATA
    ) -> Option<Box<dyn SimDataObject>> {
        event
            .into::<Com2Data>(sim)
            .map(|data| Box::new(data.clone()) as Box<dyn SimDataObject>)
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}


#[data_definition]
#[derive(Debug, Clone)]
struct Nav1Data {
    #[name = "NAV ACTIVE FREQUENCY:1"]
    #[unit = "MHz"]
    active: f64,
    #[name = "NAV STANDBY FREQUENCY:1"]
    #[unit = "MHz"]
    standby: f64,
}

impl SimDataObject for Nav1Data {
    fn get_event_data(
        &self,
        sim: &mut msfs::sim_connect::SimConnect<'_>,
        event: &SIMCONNECT_RECV_SIMOBJECT_DATA
    ) -> Option<Box<dyn SimDataObject>> {
        event
            .into::<Nav1Data>(sim)
            .map(|data| Box::new(data.clone()) as Box<dyn SimDataObject>)
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}


#[data_definition]
#[derive(Debug, Clone)]
struct Nav2Data {
    #[name = "NAV ACTIVE FREQUENCY:2"]
    #[unit = "MHz"]
    active: f64,
    #[name = "NAV STANDBY FREQUENCY:2"]
    #[unit = "MHz"]
    standby: f64,
}

impl SimDataObject for Nav2Data {
    fn get_event_data(
        &self,
        sim: &mut msfs::sim_connect::SimConnect<'_>,
        event: &SIMCONNECT_RECV_SIMOBJECT_DATA
    ) -> Option<Box<dyn SimDataObject>> {
        event
            .into::<Nav2Data>(sim)
            .map(|data| Box::new(data.clone()) as Box<dyn SimDataObject>)
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}


#[data_definition]
#[derive(Debug, Clone)]
struct XPDRData {
    #[name = "TRANSPONDER CODE:0"]
    #[unit = "BCD16"]
    code: f64,
}



impl SimDataObject for XPDRData {
    fn get_event_data(
        &self,
        sim: &mut msfs::sim_connect::SimConnect<'_>,
        event: &SIMCONNECT_RECV_SIMOBJECT_DATA
    ) -> Option<Box<dyn SimDataObject>> {
        event
            .into::<XPDRData>(sim)
            .map(|data| Box::new(data.clone()) as Box<dyn SimDataObject>)
    }

    fn as_any(&self) -> &dyn Any {
        self
    }
}

/// The possible radio devices that can be accessed
#[derive(Debug, Clone, Copy, PartialEq)]
pub enum RadioDevices {
    COM1,
    COM2,
    NAV1,
    NAV2,
    DME,
    ADF,
    XPDR,
}

/// The possible options for all of the radio devices
#[derive(Debug, PartialEq, Eq, Hash)]
pub enum RadioOptions {
    ACTIVE,
    STANDBY,
    CODE
}

#[derive(Debug)]
enum FrequencyName {
    Com1Active,
    Com1Standby,
    Com2Active,
    Com2Standby,
    Nav1Active,
    Nav1Standby,
    Nav2Active,
    Nav2Standby,
    XPDR,
}

impl FrequencyName {
    fn from_public(device: RadioDevices, option: RadioOptions) -> Result<FrequencyName, Box<dyn std::error::Error>> {
        match device {
            RadioDevices::COM1 => {
                match option {
                    RadioOptions::ACTIVE => return Ok(FrequencyName::Com1Active),
                    RadioOptions::STANDBY => return Ok(FrequencyName::Com1Standby),
                    _ => return Err(format!("Bad device, option combination {:?} with {:?}", device, option).into())
                }
            }
            RadioDevices::COM2 => {
                match option {
                    RadioOptions::ACTIVE => return Ok(FrequencyName::Com2Active),
                    RadioOptions::STANDBY => return Ok(FrequencyName::Com2Standby),
                    _ => return Err(format!("Bad device, option combination {:?} with {:?}", device, option).into())
                }
            }  
            RadioDevices::NAV1 => {
                match option {
                    RadioOptions::ACTIVE => return Ok(FrequencyName::Nav1Active),
                    RadioOptions::STANDBY => return Ok(FrequencyName::Nav1Standby),
                    _ => return Err(format!("Bad device, option combination {:?} with {:?}", device, option).into())
                }
            }  
            RadioDevices::NAV2 => {
                match option {
                    RadioOptions::ACTIVE => return Ok(FrequencyName::Nav2Active),
                    RadioOptions::STANDBY => return Ok(FrequencyName::Nav2Standby),
                    _ => return Err(format!("Bad device, option combination {:?} with {:?}", device, option).into())
                }
            }  
            RadioDevices::XPDR => {
                match option {
                    RadioOptions::CODE => return Ok(FrequencyName::XPDR),
                    _ => return Err(format!("Bad device, option combination {:?} with {:?}", device, option).into())
                }
            }  
            _ => return Err(format!("Device not found {:?} with {:?}", device, option).into())
        }
    }

    /// Return the simconnect string name of the frequency
    fn as_event(&self) -> String {
        match self {
            FrequencyName::Com1Active => "COM_RADIO_SET_HZ".to_string(),
            FrequencyName::Com1Standby => "COM_STBY_RADIO_SET_HZ".to_string(),
            FrequencyName::Com2Active => "COM2_RADIO_SET_HZ".to_string(),
            FrequencyName::Com2Standby => "COM2_STBY_RADIO_SET_HZ".to_string(),
            FrequencyName::Nav1Active => "NAV1_RADIO_SET_HZ".to_string(),
            FrequencyName::Nav1Standby => "NAV1_STBY_SET_HZ".to_string(),
            FrequencyName::Nav2Active => "NAV2_RADIO_SET_HZ".to_string(),
            FrequencyName::Nav2Standby => "NAV2_STBY_SET_HZ".to_string(),
            FrequencyName::XPDR => "XPNDR_SET".to_string()
        }
    }

    /// Get all of the enum
    pub fn all() -> &'static [FrequencyName] {
        use FrequencyName::*;
        &[
            Com1Active,
            Com1Standby,
            Com2Active,
            Com2Standby,
            Nav1Active,
            Nav1Standby,
            Nav2Active,
            Nav2Standby,
            XPDR,
        ]
    }
}

/// A data store for frequency information for consumption outside of the sim
/// scope
#[derive(Debug)]
pub struct RadioData {
    radio_type: RadioDevices,
    frequencies: HashMap<RadioOptions, f64>
}


#[derive(Debug)]
pub struct SimFreq {
    wrapper: SimWrapper
} 

impl SimFreq {
    /// Create a new object to comunicate with the sim regarding frequencies
    pub fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let data_objects: Vec<Box<dyn SimDataObject>> = vec![Box::new(Com1Data{active: 118.000, standby:118.000}), 
                                                             Box::new(Com2Data{active: 118.000, standby:118.000}),
                                                             Box::new(Nav1Data{active: 108.000, standby:108.000}),
                                                             Box::new(Nav2Data{active: 108.000, standby:108.000}),
                                                             Box::new(XPDRData{code: 7000.0})];
        let mut wrapper = SimWrapper::new("RADIO HANDLER".to_string(), data_objects)?;

        wrapper.register_data_request::<Com1Data>(0)?;
        wrapper.register_data_request::<Com2Data>(1)?;
        wrapper.register_data_request::<Nav1Data>(2)?;
        wrapper.register_data_request::<Nav2Data>(3)?;
        wrapper.register_data_request::<XPDRData>(4)?;
        
        for event_name in FrequencyName::all() {
            wrapper.register_event(event_name.as_event())?;
        }

        Ok(SimFreq { wrapper: wrapper })
    }

    /// Set a specific frequency
    /// 
    /// radio_device, radio_option - the frequency to set
    /// value - the frequency to set
    pub fn set(&mut self, radio_device: RadioDevices, radio_option: RadioOptions, value: u32) -> Result<(), Box<dyn std::error::Error>>{
        self.wrapper.write_event(FrequencyName::from_public(radio_device, radio_option)?.as_event(), value)?;

        Ok(())
    }

    /// Get frequency updates if they have occurred
    /// 
    /// # Returns
    /// A frequency update if it has occured
    pub fn get_freq_update(&mut self) -> Option<RadioData> {
        let _= self.wrapper.poll_sim();

        match self.wrapper.get_data() {
            Some(data) => {
                if let Some(data) = data.as_any().downcast_ref::<Com1Data>() {
                    let mut frequencies = HashMap::new();
                    frequencies.insert(RadioOptions::ACTIVE, data.active);
                    frequencies.insert(RadioOptions::STANDBY, data.standby);
                    return Some(RadioData{
                        radio_type: RadioDevices::COM1,
                        frequencies: frequencies
                    })
                }
                else if let Some(data) = data.as_any().downcast_ref::<Com2Data>() {
                    let mut frequencies = HashMap::new();
                    frequencies.insert(RadioOptions::ACTIVE, data.active);
                    frequencies.insert(RadioOptions::STANDBY, data.standby);
                    return Some(RadioData{
                        radio_type: RadioDevices::COM2,
                        frequencies: frequencies
                    })
                }
                else if let Some(data) = data.as_any().downcast_ref::<Nav1Data>() {
                    let mut frequencies = HashMap::new();
                    frequencies.insert(RadioOptions::ACTIVE, data.active);
                    frequencies.insert(RadioOptions::STANDBY, data.standby);
                    return Some(RadioData{
                        radio_type: RadioDevices::NAV1,
                        frequencies: frequencies
                    })
                }
                else if let Some(data) = data.as_any().downcast_ref::<Nav2Data>() {
                    let mut frequencies = HashMap::new();
                    frequencies.insert(RadioOptions::ACTIVE, data.active);
                    frequencies.insert(RadioOptions::STANDBY, data.standby);
                    return Some(RadioData{
                        radio_type: RadioDevices::NAV2,
                        frequencies: frequencies
                    })
                }
                else if let Some(data) = data.as_any().downcast_ref::<XPDRData>() {
                    let mut frequencies = HashMap::new();
                    frequencies.insert(RadioOptions::CODE, data.code);
                    return Some(RadioData{
                        radio_type: RadioDevices::XPDR,
                        frequencies: frequencies
                    })
                }

            },
            None => return None
        }
        None
    }
}

mod tests {
    use super::*;

    // #[cfg(feature = "sim_tests")]
    // #[test]
    // fn test_SimFreq_new() {
    //     let freq = SimFreq::new().expect("Check that the sim is open");
    // }

    // #[cfg(feature = "sim_tests")]
    // #[test]
    // fn test_SimFreq_set() {
    //     let mut freq = SimFreq::new().expect("Check that the sim is open");
    //     std::thread::sleep(std::time::Duration::from_secs(5));
    //     freq.set(FrequencyName::Com1Standby, 129000000).expect("bad error");
    // }

    // #[cfg(feature = "sim_tests")]
    // #[test]
    // fn test_SimWrapper_poll() {
    //     let data_objects: Vec<Box<dyn SimDataObject>> = vec![Box::new(FrequencyData{com1_active: 118.000, com1_standby:118.000})];
    //     let mut wrapper = SimWrapper::new("Frequency Communication".to_string(), data_objects).expect("Failed to open connection");

    //     let start_time = std::time::SystemTime::now();

    //     let duration = Duration::from_secs(5);
    //     let start_time = Instant::now();

    //     while Instant::now().duration_since(start_time) < duration {
    //         wrapper.poll_sim().expect("Failed to poll");
    //     }
    // }
}