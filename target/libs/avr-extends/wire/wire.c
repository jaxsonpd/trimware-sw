/**
 * @file i2c.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-10-20
 * @brief Definitions for the I2C 2 wire driver
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <avr/io.h>

#include "../avr_extends/wire.h"

#define START_SENT 0x08
#define STARTR_SENT 0x10

#define ADDRESS_ACK 0x18
#define DATA_ACK 0x28

int wire_init(uint32_t scl_freq) {
    // Calculate TWI interface speed
    int TWBR_value = (F_CPU - scl_freq * 16UL) / (2 * scl_freq * 1);
    TWBR = TWBR_value;

    return 0;
}

static int send_start(void) {
    // Enable two wire interface clear interrupt 
    // and send a start condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // Wait for start condition to occur and check correct
    while (!(TWCR & (1 << TWINT)));

    // if (((TWSR & 0xF8)!= START_SENT) && ((TWSR & 0xF8)!= STARTR_SENT)) {
    //     return 1;
    // }
    return 0;
}

static int send_restart(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWSTO) | (1 << TWEN);
    
    while (!(TWCR & (1 << TWINT)));

}

static int send_address(uint8_t addr, bool read) {
    TWDR = addr << 1 | read;
    TWCR = (1 << TWINT) | (1 << TWEN);

    // Wait for data to send and check acknowledge
    while (!(TWCR & (1 << TWINT)));

    // if ((TWSR & 0xF8) != ADDRESS_ACK) {
    //     return 2;
    // }

    return 0;
}

static int send_byte(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);

    // Wait for data to send and check acknowledge
    while (!(TWCR & (1 << TWINT)));

    // if ((TWSR & 0xF8) != DATA_ACK) {
    //     return 2;
    // }

    return 0;
}

static int read_byte(uint8_t *data) {
    uint64_t x = 0;
    TWCR = (1 << TWINT) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)) && x < 1000) {
        x++;
        asm ("");
    }

    *data = TWDR;

    return 0;
}

static int send_stop(void) {
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

    return 0;
}

int wire_write(uint8_t addr, uint8_t data) {
    int result = 0;
    // Enable two wire interface clear interrupt 
    // and send a start condition
    result = send_start();
    if (result != 0) {
        return result;
    }
    // load the address with a write cmd and trigger a send
    result = send_address(addr, false);
    if (result != 0) {
        return result;
    }

    // Load and send data
    result = send_byte(data);
    if (result != 0) {
        return result;
    }

    // Send stop
    result = send_stop();
    if (result != 0) {
        return result;
    }

    return 0;
}

int wire_write_reg(uint8_t addr, uint8_t reg, uint8_t data) {
    int result = 0;

    result = send_start();
    if (result != 0) return result;

    // load the address with a write cmd and trigger a send
    result = send_address(addr, false);
    if (result != 0) return result;

    // Load and send data
    result = send_byte(reg);
    if (result != 0) return result;

    result = send_byte(data);
    if (result != 0) return result;

    // Send stop
    result = send_stop();
    if (result != 0) return result;

    return 0;
}

int wire_read_reg(uint8_t addr, uint8_t reg, uint8_t buf[], uint8_t len) {
    int result = 0;

    result = send_start();
    if (result != 0) return result;

    // load the address with a write cmd and trigger a send
    result = send_address(addr, false);
    if (result != 0) return result;

    result = send_byte(reg);
    if (result != 0) return result;

    // result = send_stop();
    result = send_start();
    // send_restart();

    result = send_address(addr, true);

    
    for (size_t i = 0; i < len; i++)
    {
        result = read_byte(&buf[i]);
        if (result != 0) return result;
    }

    result = send_stop();
    if (result != 0) return result;

    return 0;
    

}
