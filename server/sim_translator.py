## 
# @file sim_translator.py
# @author Jack Duignan (JackpDuignan@gmail.com)
# @date 2024-11-30
# @brief The main file for the simulation translation program

from SimConnect import *
from com_managment import ComRadio
from serial_interface import init_serial, get_commands
from time import sleep

def main():
    init_serial()

    try:
        msfs2020 = SimConnect()
    except ConnectionError:
        print("Failed to connect to simulator make sure it is launched")
        return
    
    aircraftReq = AircraftRequests(msfs2020, _time=200)
    aircraftEvent = AircraftEvents(msfs2020)

    radio = ComRadio(aircraftReq, aircraftEvent)

    while True:
        print(radio.get_active_freq(1))
        commands = get_commands()
        if (commands != []):
            print(commands)

        if(b"EVENT_RADIO_1_SWAP_ACTIVE\r\n" in commands):
            print("increased frequency")
            radio.inc_freq(1)
        sleep(0.5)

if __name__ == "__main__":
    main()