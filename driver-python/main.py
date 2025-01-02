import serial
import time

import serial
import threading
import time

def read_from_serial(ser: serial.Serial):
    """Thread function to continuously read from the serial port."""
    while True:
        try:
            # Read available data
            if ser.in_waiting > 0:
                response = ser.read_until(b"\n")
                print(f"{time.time()} Received: {response}")
        except Exception as e:
            print(f"Error while reading: {e}")
            break

def main():
    # Configuration for the serial port
    port_name = "/dev/ttyUSB0"  # Replace with your serial port name
    baud_rate = 115200    # Adjust based on your device's settings
    timeout = 20         # Timeout in seconds for reading

    # Create the serial connection
    try:
        ser = serial.Serial(port=port_name, baudrate=baud_rate, timeout=timeout, stopbits=serial.STOPBITS_ONE, parity=serial.PARITY_NONE)
        print(f"Connected to {port_name} at {baud_rate} baud.")
    except Exception as e:
        print(f"Failed to connect to {port_name}: {e}")
        return

    # Start the reading thread
    reader_thread = threading.Thread(target=read_from_serial, args=(ser,), daemon=True)
    reader_thread.start()

    try:
        while True:
            time.sleep(5)
            # Get user input for the hex string to send
            hex_string = b'\x7E\x00\x00\xFF\xFF\x7E'  # Replace with your data

            try:
                # Convert the hex string to bytes and send it
                print(f"Sending: {hex_string}")
                for byte in hex_string:
                    ser.write(bytes([byte]))
                    time.sleep(0.01)

            except ValueError:
                print("Invalid hex string. Please try again.")
    except KeyboardInterrupt:
        print("Exiting program...")
    finally:
        ser.close()
        print("Serial connection closed.")

if __name__ == "__main__":
    main()
