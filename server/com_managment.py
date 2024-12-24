## 
# @file com_managment.py
# @author Jack Duignan (JackpDuignan@gmail.com)
# @date 2024-11-30
# @brief Manage the com radios using the simconnect api

from SimConnect import *

class ComRadio():
    """
    A class to allow for communication with the com radio
    """
    def __init__(self, aircraftReq: AircraftRequests, aircraftEvent: AircraftEvents):
        self.aircraftReq = aircraftReq
        self.aircraftEvent = aircraftEvent
        pass

    def get_active_freq(self, com: int) -> float:
        return self.aircraftReq.get(f"COM_ACTIVE_FREQUENCY:{com}")

    def inc_freq(self, com: int):
        incFine = self.aircraftEvent.find("COM_RADIO_FRACT_INC_CARRY")
        # incFine = self.aircraftEvent.find("AP_MASTER")

        incFine()
