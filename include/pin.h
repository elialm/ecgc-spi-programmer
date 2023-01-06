#ifndef __PIN_H__
#define __PIN_H__

#include <inttypes.h>

#define pin_define(letter, number) pin_create(&PORT ## letter, &PIN ## letter, &DDR ## letter, number)

typedef struct pin {
    volatile uint8_t *port;
    volatile uint8_t *pin;
    volatile uint8_t *ddr;
    uint8_t number;
} pin_t;

typedef enum pin_direction {
    PIN_INPUT = 0,
    PIN_OUTPUT = 1
} pin_direction_t;

#ifdef __cplusplus
extern "C" {
#endif

pin_t pin_create(volatile uint8_t *port, volatile uint8_t *pin, volatile uint8_t *ddr, uint8_t number);
void pin_direction(pin_t *pin, pin_direction_t dir);
void pin_set(pin_t *pin);
void pin_clear(pin_t *pin);

#ifdef __cplusplus
}
#endif

#endif //__PIN_H__