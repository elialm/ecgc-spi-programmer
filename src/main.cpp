#include <Arduino.h>

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define MOSI PB3
#define MISO PB4
#define SS PB2
#define SCK PB5

void spi_write(uint8_t data) {
  SPDR = data;
}

uint8_t spi_read() {
  while (!(SPSR & (1 << SPIF))) {
    delay(5);
  }

  return SPDR;
}

void spi_slave_init()
{
	// set MISO to output and rest to input
	SPI_DDR |= (1 << MISO);
	SPI_DDR &= ~(1 << MOSI) & ~(1 << SCK) & ~(1 << SS);
	
	// set MISO to HIGH
	SPI_PORT |= (1 << MISO);
	
	// enable SPI
	SPCR |= (1 << SPE);
}

void setup() {
  Serial.begin(115200);
  spi_slave_init();
}

void loop() {
  uint8_t spi_data = spi_read();
  spi_write(~spi_data);

  Serial.print("Received: ");
  Serial.println(spi_data, HEX);
}
