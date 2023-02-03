#ifndef __SPI_DEBUGGER_H__
#define __SPI_DEBUGGER_H__

#include "pin.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

int8_t spi_debugger_init(pin_t *cs_pin, pin_t *dbg_pin);
void spi_debugger_deinit(pin_t *cs_pin, pin_t *dbg_pin);
int8_t spi_debugger_set_auto_increment(pin_t *cs_pin, pin_t *dbg_pin, const bool enable);
int8_t spi_debugger_set_address(pin_t *cs_pin, pin_t *dbg_pin, uint16_t address);
int8_t spi_debugger_write(pin_t *cs_pin, pin_t *dbg_pin, const uint8_t *data, size_t data_length);
int8_t spi_debugger_read(pin_t *cs_pin, pin_t *dbg_pin, uint8_t *data, const size_t data_length);

#ifdef __cplusplus
}
#endif

#endif //__SPI_DEBUGGER_H__