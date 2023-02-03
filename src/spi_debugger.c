#include "spi_debugger.h"

#include "byte_regex.h"
#include "spi.h"

#include <string.h>

static int __spi_debugger_send_packet(const uint8_t *write_data, uint8_t *read_data, size_t data_length, const uint8_t *response_pattern)
{
    // send write_data and read into read_data
    for (size_t i = 0; i < data_length; i++) {
        read_data[i] = spi_master_tx(write_data[i]);
    }

    return byte_regex_match(response_pattern, read_data, data_length);
}

int8_t spi_debugger_init(pin_t *cs_pin, pin_t *dbg_pin)
{
    int8_t err;

    pin_direction(cs_pin, PIN_OUTPUT);
    pin_direction(dbg_pin, PIN_OUTPUT);

    pin_set(cs_pin);
    pin_set(dbg_pin);

    spi_master_init(SPEED_16);

    // send idle command to check if core is enabled and flush any ongoing transaction
    // last byte must be idle response (0xF1)
    uint8_t flush_response;
    for (size_t i = 0; i < 18; i++) {
        flush_response = spi_master_tx(0x0F);
    }

    if (flush_response != 0xF1) {
        return -1;
    }

    if ((err = spi_debugger_set_auto_increment(cs_pin, dbg_pin, false)) != 0) {
        return -2;
    }

    if ((err = spi_debugger_set_address(cs_pin, dbg_pin, 0x0000)) != 0) {
        return -3;
    }

    return 0;
}

void spi_debugger_deinit(pin_t *cs_pin, pin_t *dbg_pin)
{
    pin_clear(dbg_pin);
}

int8_t spi_debugger_set_auto_increment(pin_t *cs_pin, pin_t *dbg_pin, const bool enable)
{
    int err;
    uint8_t read_buffer[2];

    if (enable) {
        // enable auto increment command
        if ((err = __spi_debugger_send_packet("\x04\x0F", read_buffer, 2, "\x11\xF1\x41")) != 0) {
            return err;
        }
    } else {
        // disable auto increment command
        if ((err = __spi_debugger_send_packet("\x05\x0F", read_buffer, 2, "\x11\xF1\x51")) != 0) {
            return err;
        }
    }

    return 0;
}

int8_t spi_debugger_set_address(pin_t *cs_pin, pin_t *dbg_pin, uint16_t address)
{
    int err;
    uint8_t read_buffer[2];
    uint8_t address_data[2] = { 0x00, 0x0F };
    uint8_t address_response_pattern[4] = { 0x11, 0x00, 0x00, 0x00 };
    uint8_t address_high = (address >> 8) & 0xFF;
    uint8_t address_low = address & 0xFF;

    // set high address command
    if ((err = __spi_debugger_send_packet("\x03\x0F", read_buffer, 2, "\x11\xF1\x31")) != 0) {
        return -1;
    }

    // set high address data
    address_data[0] = address_high;
    address_response_pattern[2] = address_high;
    if ((err = __spi_debugger_send_packet(address_data, read_buffer, 2, address_response_pattern)) != 0) {
        return -2;
    }

    // set low address command
    if ((err = __spi_debugger_send_packet("\x02\x0F", read_buffer, 2, "\x11\xF1\x21")) != 0) {
        return -3;
    }

    // set low address data
    address_data[0] = address_low;
    address_response_pattern[2] = address_low;
    if ((err = __spi_debugger_send_packet(address_data, read_buffer, 2, address_response_pattern)) != 0) {
        return -4;
    }

    return 0;
}

int8_t spi_debugger_write(pin_t *cs_pin, pin_t *dbg_pin, const uint8_t *data, size_t data_length)
{
    int err;
    size_t data_index = 0;
    size_t burst_amount = data_length / 16;
    size_t individual_amount = data_length % 16;
    uint8_t read_buffer[16];

    uint8_t burst_response_pattern[18];
    burst_response_pattern[0] = 0xF1;
    burst_response_pattern[1] = 0x00;
    // close_response_pattern[2..16] is set in burst loop
    burst_response_pattern[17] = 0x00;
    uint8_t close_response_pattern[4];
    close_response_pattern[0] = 0x11;
    // close_response_pattern[1] is set in burst loop
    close_response_pattern[2] = 0xF1;
    close_response_pattern[3] = 0x00;

    uint8_t individual_data[2];
    // individual_data[0] is set in individual loop
    individual_data[1] = 0x0F;
    uint8_t individual_response_pattern[4];
    individual_response_pattern[0] = 0x11;
    individual_response_pattern[1] = 0x00;
    // individual_response_pattern[2] is set in individual loop
    individual_response_pattern[3] = 0x00;

    // write all bursts
    for (size_t i = 0; i < burst_amount; i++) {
        // write burst command
        if ((err = __spi_debugger_send_packet("\x0B\x0F", read_buffer, 2, "\x11\xF1\xB1")) != 0) {
            return -1;
        }

        // write burst data
        memcpy(burst_response_pattern + 2, data + data_index, 15);
        if ((err = __spi_debugger_send_packet(data + data_index, read_buffer, 16, burst_response_pattern)) != 0) {
            return -2;
        }

        // write burst close off
        burst_response_pattern[1] = data[data_index + 15];
        if ((err = __spi_debugger_send_packet("\x0F\x0F", read_buffer, 2, burst_response_pattern)) != 0) {
            return -3;
        }

        data_index += 16;
    }

    // write all individuals
    for (size_t i = 0; i < individual_amount; i++, data_index++) {
        // write command
        if ((err = __spi_debugger_send_packet("\x09\x0F", read_buffer, 2, "\x11\xF1\x91")) != 0) {
            return -4;
        }

        // write data
        individual_data[0] = data[data_index];
        individual_response_pattern[2] = data[data_index];
        if ((err = __spi_debugger_send_packet(individual_data, read_buffer, 2, individual_response_pattern)) != 0) {
            return -5;
        }
    }

    return 0;
}

int8_t spi_debugger_read(pin_t *cs_pin, pin_t *dbg_pin, uint8_t *data, const size_t data_length)
{
    return -1;
}