#include "message_parser.h"
#include "spi.h"

#include <Arduino.h>
#include <string.h>

static const char hex_digits[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

bool is_hex_digit(const char digit)
{
    return (digit >= '0' && digit <= '9')
        || (digit >= 'A' && digit <= 'F')
        || (digit >= 'a' && digit <= 'f');
}

uint8_t parse_hex_nibble(const char n)
{
    switch (n) {
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3': 
            return 3;
        case '4': 
            return 4;
        case '5': 
            return 5;
        case '6': 
            return 6;
        case '7': 
            return 7;
        case '8': 
            return 8;
        case '9': 
            return 9;
        case 'A':
        case 'a':
            return 10;
        case 'B':
        case 'b':
            return 11;
        case 'C':
        case 'c':
            return 12;
        case 'D':
        case 'd':
            return 13;
        case 'E':
        case 'e':
            return 14;
        case 'F':
        case 'f':
            return 15;
        default: return 0;
    }
}

uint8_t parse_hex_byte(const char h, const char l)
{
    uint8_t parsed_h = parse_hex_nibble(h) << 4;
    uint8_t parsed_l = parse_hex_nibble(l);

    return (parsed_h & 0xF0) | (parsed_l & 0x0F);
}

void handle_write(const char* message)
{
    uint8_t length = strlen(message);
    uint8_t data = 0;

    if (length != 3) {
        goto error;
    }

    if (!is_hex_digit(message[1]) || !is_hex_digit(message[2])) {
        goto error;
    }

    data = parse_hex_byte(message[1], message[2]);
    spi_master_tx(data);
    data = spi_master_rx();

    Serial.print("#R");
    Serial.print(hex_digits[(data >> 4) & 0x0F]);
    Serial.print(hex_digits[data & 0x0F]);
    Serial.println(';');

    return;

    error:
    Serial.println("#EINV;");
    return;
}

uint8_t parser_read_cmd(const char* message)
{
    switch (message[0]) {
        case 'W':
            handle_write(message);
            break;

        default:
            Serial.println("Received unknown command");
            break;
    }

    return 0;
}

void setup() {
    Serial.begin(115200);
    spi_master_init(SPEED_32);
}

void loop() {
    if (Serial.available()) {
        parser_read_letter(Serial.read());
    }

    delay(5);
}
