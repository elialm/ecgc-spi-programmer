#include "pin.h"

pin_t pin_create(volatile uint8_t *port, volatile uint8_t *pin, volatile uint8_t *ddr, uint8_t number)
{
    pin_t pin_struct;

    pin_struct.port = port;
    pin_struct.pin = pin;
    pin_struct.ddr = ddr;
    pin_struct.number = number;

    pin_direction(&pin_struct, PIN_INPUT);
    pin_clear(&pin_struct);

    return pin_struct;
}

void pin_direction(pin_t *pin, pin_direction_t dir)
{
    if (dir == PIN_INPUT) {
        *(pin->ddr) &= ~(1 << pin->number);
    } else if (dir == PIN_OUTPUT) {
        *(pin->ddr) |= 1 << pin->number;
    }
}

void pin_set(pin_t *pin)
{
    *(pin->port) |= 1 << pin->number;
}

void pin_clear(pin_t *pin)
{
    *(pin->port) &= ~(1 << pin->number);
}
