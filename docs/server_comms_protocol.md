# Server Communications Protocol

See device/target/libs/custom_can_protocol/README.md for more information/

### Radio Devices

- COM1 = 0
  - u32 frequency in kHz, active and standby, 108 to 137.999 MHz, 25KHz step
- COM2
  - u32 frequency in kHz, active and standby, 108 to 137.999 MHz, 25KHz step
- NAV1
  - u32 frequency in kHz, active and standby, 108.0 to 117.95 MHz, 50KHz step
- NAV2
  - u32 frequency in kHz, active and standby, 108.0 to 117.95 MHz, 50KHz step
- ADF
- DME
- XPDR
  - 4 digit BCD only active. Octet limited


## Protocol Definitions

### Commands

| Identifier | Function | Direction |
| - | - | - |
| 0x01 | Update Frequencies | Driver-MCU |
| 0x04 | Rotary switch state | MCU to Driver |

### Frequency Update

Command: Update frequencies

Bytes:

- Radio Device Type
- The first 4 bytes are the active frequency MSB first
- The next 4 bytes are the standby frequency MSB first

The frequency can be requested by just sending the device type byte.

### Rotary Switch State

Command: Selected Radio Device

bytes:

- Radio Device Type

The device type can be requested by just sending an empty packet.
