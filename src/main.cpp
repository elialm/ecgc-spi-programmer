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

uint8_t compose_hex_nibble(const uint8_t n)
{
    return hex_digits[n & 0x0F];
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
    Serial.print(compose_hex_nibble(data >> 4));
    Serial.print(compose_hex_nibble(data));
    Serial.println(';');

    return;

    error:
    Serial.println("#EINV;");
    return;
}

void handle_burst_write(const char* message)
{
    uint8_t length = strlen(message);
    uint8_t byte_length = length / 2;
    uint8_t data;
    char read_message[17];

    // message format is Bxxyyzz... where the lower case letters are some hex byte.
    // the maximum number of bursts is 8 bytes, hence the max size of 17
    // 1 command char + 2 chars/byte * nr. of bytes
    if (length % 2 == 0 || length < 3 || length > 17) {
        goto error;
    }

    for (uint8_t i = 1; i < length; i++) {
        if (!is_hex_digit(message[i])) {
            goto error;
        }
    }

    for (uint8_t i = 0; i < byte_length; i++) {
        uint8_t i_doubled = i * 2;
        data = parse_hex_byte(message[i_doubled + 1], message[i_doubled + 2]);
    
        spi_master_tx(data);
        data = spi_master_rx();

        read_message[i_doubled] = compose_hex_nibble(data >> 4);
        read_message[i_doubled + 1] = compose_hex_nibble(data);
        read_message[i_doubled + 2] = '\0';
    }

    Serial.print("#R");
    Serial.print(read_message);
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

        case 'B':
            handle_burst_write(message);
            break;

        default:
            Serial.println("#EPERM;");
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

    if (!Serial.available()) {
        delay(10);
    }
}
