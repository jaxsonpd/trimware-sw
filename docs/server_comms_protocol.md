# Server Communications Protocol

See device/target/libs/custom_can_protocol/README.md for more information/

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
- The first 4 bytes are the active frequency
- The next 4 bytes are the standby frequency

The frequency can be requested by just sending the device type byte.

### Rotary Switch State

Command: Selected Radio Device

bytes:

- Radio Device Type
  - COM1 = 0
  - COM2
  - NAV1
  - NAV2
  - ADF
  - DME
  - XPDR

The device type can be requested by just sending an empty packet.
