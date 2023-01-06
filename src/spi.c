#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include <stdbool.h>

static void (*spi_isr)(void) = NULL;
static uint8_t master_response = 0;
static bool response_enable = false;

ISR(SPI_STC_vect) 
{
	if (response_enable)
	{
		SPDR = master_response;
		response_enable = false;
	}
	else
	{
		SPDR = 0xFF;
	}
	
	spi_isr();
}

void spi_master_init(uint8_t freq)
{
	// set SPI_SS, SPI_MOSI and SPI_SCK as output and MISO as input
	SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);
	SPI_DDR &= ~(1 << SPI_MISO);
	
	// pullup MISO line
	SPI_PORT |= (1 << SPI_SS);
	
	// enable spi, select master and set frequency
	if (READ_BIT(freq, 3)) 
	{
		SPSR |= (1 << SPI2X);
		freq &= ~0x03;
	}
	SPCR |= (1 << SPE) | (1 << MSTR) | (freq << SPR0);

	// set default SPI mode
	SPCR &= ~(1 << CPOL) & ~(1 << CPHA) & ~(1 << DORD);
}

void spi_master_tx(uint8_t data)
{
	// set spi data register to contain data to send
	SPDR = data;
	
	// wait until data has been sent
	while (!(SPSR & (1 << SPIF))) {}
}

uint8_t spi_master_rx()
{
	// spi_master_tx(0xFF);
	return SPDR;
}

void spi_slave_init()
{
	// set MISO to output and rest to input
	SPI_DDR |= (1 << SPI_MISO);
	SPI_DDR &= ~(1 << SPI_MOSI) & ~(1 << SPI_SCK) & ~(1 << SPI_SS);
	
	// set MISO to HIGH
	SPI_PORT |= (1 << SPI_MISO);
	
	// enable SPI
	SPCR |= (1 << SPE);
}

uint8_t spi_slave_rx()
{
	if (SPCR & (1 << SPIE)) return 0;
	
	while (!(SPSR & (1 << SPIF)));
	
	return SPDR;
}

int8_t spi_slave_tx(uint8_t data)
{
	if (response_enable) return -1;
	
	master_response = data;
	response_enable = true;
	
	return 0;
}

void spi_attach_interrupt(void (*func)(void))
{
	SPCR |= (1 << SPIE);
	
	spi_isr = func;
	
	sei();
}

void spi_detach_interrupt()
{
	SPCR &= ~(1 << SPIE);
	
	spi_isr = NULL;
}
