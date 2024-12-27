# Server Communications Protocol

## Possible Options

CBOR - Lots of serial overhead - easy processing
CAN like - custom protocol - lightweight

Currently running with a CAN like protocol due to simplicity if a higher bandwidth
stream/faster CPU was available then CBOR is a better solution.

## Protocol Definitions

### Packet Structure

| Field | Length | Description |
| - | - | - |
| Start Byte `0x7E` | 1 Byte | Frame Delimiter |
| Identifier | 1 Byte | Command Identifier |
| Payload Length | 1 Byte | Length of the Payload |
| Payload | N Byte | The data |
| CRC | 2 Byte | CRC-16 for error checking |

### Commands

| Identifier | Function | Direction |
| - | - | - |
| 0x01 | Update Frequencies | Driver-MCU |
| 0x02 | Encoder adjustment data | MCU to Driver |
| 0x03 | Freq swap button state | MCU to Driver |
| 0x04 | Rotary switch state | MCU to Driver |
| 0x05 | LED Toggle | MCU to Driver |
| 0xFE | re-send last command | both |
| 0xFF | Heartbeat (keep-alive) | Both |

### Frequency Update

Command: Update frequencies
Data: Current frequency 123450 and standby frequency 124250

```yaml
Start Byte:      0x7E
Identifier:      0x01
Payload Length:  6
Payload:         [0x01, 0xE2, 0x40, 0x01, 0xE5, 0x02] (frequencies)
CRC:             0x1234
```

### Encoder Adjustment

Command: Encoder adjustment
Data: Fine adjustment +5, Coarse adjustment -3

Packet Example:

```yaml
Copy code
Start Byte:      0x7E
Identifier:      0x02
Payload Length:  2
Payload:         [0x05, 0xFD] (fine=+5, coarse=-3)
CRC:             0x5678
```

### Button States

Command: Button state updates
Data: Button 1 pressed, Button 2 not pressed

Packet Example:

```yaml
Copy code
Start Byte:      0x7E
Identifier:      0x03
Payload Length:  2
Payload:         [0x01, 0x00] (buttons=[1, 0])
CRC:             0x4321
```


### Rotary Switch State

Command: Rotary switch state
Data: Switch in position 2

```yaml
Copy code
Start Byte:      0x7E
Identifier:      0x04
Payload Length:  1
Payload:         [0x02] (switch position)
CRC:             0x6789
```

### CRC

Use the CR-16 Checksum with the 0x8005 polynomial.