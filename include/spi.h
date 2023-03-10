/*
 * spi.h
 *
 * Created: 23-Mar-19 12:07:01
 *  Author: user
 */ 

#include <avr/io.h>

#ifndef SPI_H_
#define SPI_H_

#define SPEED_2 0b100
#define SPEED_4 0b000
#define SPEED_8 0b101
#define SPEED_16 0b001
#define SPEED_32 0b110
#define SPEED_64 0b010
#define SPEED_128 0b011

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define SPI_MOSI PB3
#define SPI_MISO PB4
#define SPI_SS PB2
#define SPI_SCK PB5

#define SELECT() SPI_PORT &= ~(1 << SS);
#define DESELECT() SPI_PORT |= (1 << SS);

#ifndef READ_BIT
#define READ_BIT(number, index) (((number) & (1 << (index))) >> (index))
#endif

#ifdef __cplusplus
extern "C" {
#endif

void spi_master_init(uint8_t freq);
uint8_t spi_master_tx(uint8_t data);

void spi_slave_init();
uint8_t spi_slave_rx();
int8_t spi_slave_tx(uint8_t data);

void spi_attach_interrupt(void (*func)(void));
void spi_detach_interrupt();

#ifdef __cplusplus
}
#endif

#endif /* SPI_H_ */