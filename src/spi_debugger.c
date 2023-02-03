#include "spi_debugger.h"

#include "byte_regex.h"
#include "spi.h"

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
    return -1;
}

int8_t spi_debugger_write(pin_t *cs_pin, pin_t *dbg_pin, const uint8_t *data, size_t data_length)
{
    return -1;
}

int8_t spi_debugger_write(pin_t *cs_pin, pin_t *dbg_pin, uint8_t *data, const size_t data_length)
{
    return -1;
}