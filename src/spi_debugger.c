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
    uint8_t err;

    pin_direction(cs_pin, PIN_OUTPUT);
    pin_direction(dbg_pin, PIN_OUTPUT);

    pin_set(cs_pin);
    pin_set(dbg_pin);

    spi_master_init(SPEED_16);

    if ((err = spi_debugger_set_auto_increment(cs_pin, dbg_pin, false)) != 0) {
        return err;
    }

    if ((err = spi_debugger_set_address(cs_pin, dbg_pin, 0x0000)) != 0) {
        return err;
    }

    return 0;
}

void spi_debugger_deinit(pin_t *cs_pin, pin_t *dbg_pin)
{
    pin_clear(dbg_pin);
}

int8_t spi_debugger_set_auto_increment(pin_t *cs_pin, pin_t *dbg_pin, const bool enable)
{
    return -1;
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