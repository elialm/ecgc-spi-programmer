#include "spi_debugger.h"
#include "spi.h"

uint8_t spi_debugger_init(pin_t *cs_pin, pin_t *dbg_pin)
{
    pin_direction(cs_pin, PIN_OUTPUT);
    pin_direction(dbg_pin, PIN_OUTPUT);

    pin_set(cs_pin);
    pin_set(dbg_pin);

    spi_master_init(SPEED_16);

    return -1;
}

void spi_debugger_deinit(pin_t *cs_pin, pin_t *dbg_pin)
{
    pin_clear(dbg_pin);
}

uint8_t spi_debugger_set_auto_increment(pin_t *cs_pin, pin_t *dbg_pin, const bool enable)
{
    return -1;
}

uint8_t spi_debugger_set_address(pin_t *cs_pin, pin_t *dbg_pin, uint16_t address)
{
    return -1;
}

uint8_t spi_debugger_write(pin_t *cs_pin, pin_t *dbg_pin, const uint8_t *data, size_t data_length)
{
    return -1;
}

uint8_t spi_debugger_write(pin_t *cs_pin, pin_t *dbg_pin, uint8_t *data, const size_t data_length)
{
    return -1;
}