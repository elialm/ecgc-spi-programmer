#include "pin.h"
#include "spi_debugger.h"

#include "message_parser.h"

#include <Arduino.h>
#include <string.h>

#define DEBUGGER_DATA_BUFFER_SIZE 32

static const char hex_digits[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

pin_t cs_pin = pin_define(B, 2);
pin_t dbg_pin = pin_define(C, 0);

bool is_hex_digit(const char digit)
{
    return (digit >= '0' && digit <= '9')
        || (digit >= 'A' && digit <= 'F')
        || (digit >= 'a' && digit <= 'f');
}

char compose_hex_nibble(const uint8_t n)
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

void serial_println(const char *msg)
{
    Serial.print(msg);
    Serial.print('\n');
}

uint8_t min_uint8(uint8_t a, uint8_t b) {
    return a < b ? a : b;
}

const char *get_error_str(const char **str_table, int8_t error_code)
{
    return str_table[(error_code * -1) - 1];
}

uint16_t write_amount = 0;

static const char *error_str_write[] = {
    "EWBC",
    "EWBD",
    "EWBO",
    "EWC",
    "EWD"
};

void handle_write_data(const char *message)
{
    int8_t err;
    uint8_t length = strlen(message);

    if (length < 3 || length > 65 || length % 2 != 1) {
        serial_println("EINV");
        return;
    }

    for (uint8_t i = 1; i < length; i++) {
        if (!is_hex_digit(message[i])) {
            serial_println("EINV");
            return;
        }
    }

    uint16_t burst_amount = (length - 1) / 2;
    if (burst_amount > write_amount) {
        serial_println("EWROVER");
        return;
    }

    // parse data into write buffer
    uint8_t write_buffer[DEBUGGER_DATA_BUFFER_SIZE];
    for (uint8_t write_index = 0, message_index = 1; write_index < burst_amount; write_index++, message_index+=2) {
        write_buffer[write_index] = parse_hex_byte(message[message_index], message[message_index + 1]);
    }

    // send parsed data to be written
    err = spi_debugger_write(&cs_pin, &dbg_pin, write_buffer, burst_amount);
    if (err != 0) {
        serial_println(get_error_str(error_str_write, err));
        return;
    }

    serial_println("ACK");
    return;
}

void handle_write_command(const char *message)
{
    uint8_t length = strlen(message);

    if (length != 3) {
        serial_println("EINV");
        return;
    }

    if (!is_hex_digit(message[1]) || !is_hex_digit(message[2])) {
        serial_println("EINV");
        return;
    }

    if (write_amount != 0) {
        serial_println("EBUSY");
        return;
    }

    write_amount = parse_hex_byte(message[1], message[2]) + 1;

    serial_println("ACK");
    return;
}

static const char *error_str_read[] = {
    "ERBC",
    "ERBZ",
    "ERC",
    "ERZ"
};

void handle_read(const char *message)
{
    int8_t err;
    uint8_t length = strlen(message);

    if (length != 3) {
        serial_println("EINV");
        return;
    }

    if (!is_hex_digit(message[1]) || !is_hex_digit(message[2])) {
        serial_println("EINV");
        return;
    }

    uint16_t read_amount = parse_hex_byte(message[1], message[2]) + 1;
    uint8_t read_buffer[DEBUGGER_DATA_BUFFER_SIZE];

    for (uint16_t i = 0; i < read_amount; i+=DEBUGGER_DATA_BUFFER_SIZE) {
        uint8_t read_length = min_uint8(read_amount - i, DEBUGGER_DATA_BUFFER_SIZE);
        err = spi_debugger_read(&cs_pin, &dbg_pin, read_buffer, read_length);

        if (err != 0) {
            serial_println(get_error_str(error_str_read, err));
            return;
        }

        // send read data over serial
        Serial.print('D');
        for (uint8_t j = 0; j < read_length; j++) {
            Serial.print(compose_hex_nibble(read_buffer[j] >> 4));
            Serial.print(compose_hex_nibble(read_buffer[j]));
        }
        Serial.print('\n');
    }

    return;
}

void handle_set_auto_increment(const char *message)
{
    int8_t err;
    bool enable_auto_increment;
    uint8_t length = strlen(message);

    if (length != 2) {
        goto error;
    }

    switch (message[1]) {
        case 'E':
            enable_auto_increment = true;
            break;

        case 'D':
            enable_auto_increment = false;
            break;

        default:
            goto error;
    }

    err = spi_debugger_set_auto_increment(&cs_pin, &dbg_pin, enable_auto_increment);
    if (err != 0) {
        serial_println("EAIC");
        return;
    }

    serial_println("ACK");
    return;

    error:
    serial_println("EINV");
    return;
}

static const char *error_str_set_address[] = {
    "EHAC",
    "EHAD",
    "ELAC",
    "ELAD"
};

void handle_set_address(const char *message)
{
    uint8_t length = strlen(message);
    int8_t err;
    uint16_t address = 0;

    if (length != 5) {
        goto error;
    }

    for (uint8_t i = 1; i < length; i++) {
        if (!is_hex_digit(message[i])) {
            goto error;
        }
    }
    
    address |= parse_hex_byte(message[1], message[2]) << 8;
    address |= parse_hex_byte(message[3], message[4]);

    err = spi_debugger_set_address(&cs_pin, &dbg_pin, address);
    if (err != 0) {
        serial_println(get_error_str(error_str_set_address, err));
        return;
    }

    serial_println("ACK");
    return;

    error:
    serial_println("EINV");
    return;
}

static const char *error_str_enable[] = {
    "EFLUSH",
    "EINCD",
    "EADDR"
};

void handle_core_enable(const char *message)
{
    int8_t err;
    uint8_t length = strlen(message);

    if (length != 2) {
        goto error;
    }

    switch (message[1]) {
        case 'E':
            err = spi_debugger_init(&cs_pin, &dbg_pin);
            if (err != 0) {
                serial_println(get_error_str(error_str_enable, err));
                return;
            }
            break;

        case 'D':
            spi_debugger_deinit(&cs_pin, &dbg_pin);
            break;

        default:
            goto error;
    }

    serial_println("ACK");
    return;

    error:
    serial_println("EINV");
    return;
}

void handle_serial_data(const char *message)
{
    switch (message[0]) {
        // SET_ADDR_H and SET_ADDR_L
        case 'A':
            handle_set_address(message);
            break;

        // AUTO_INC_EN and AUTO_INC_DIS
        case 'I':
            handle_set_auto_increment(message);
            break;

        // READ
        case 'R':
            handle_read(message);
            break;

        // WRITE
        case 'W':
            handle_write_command(message);
            break;

        // DATA
        case 'D':
            handle_write_data(message);
            break;

        // CORE_ENABLE and CORE_DISABLE
        case 'C':
            handle_core_enable(message);
            break;

        default:
            serial_println("NOCMD");
            break;
    }
}

void handle_serial_overload()
{
    serial_println("BUFOVER");
}

void setup() {
    parser_set_message_handler(handle_serial_data);
    parser_set_overload_handler(handle_serial_overload);

    pin_direction(&cs_pin, PIN_OUTPUT);
    pin_direction(&dbg_pin, PIN_OUTPUT);

    pin_set(&cs_pin);
    pin_clear(&dbg_pin);

    // Indicate that programmer is ready
    Serial.begin(115200);
    serial_println("RDY");
}

void loop() {
    if (Serial.available()) {
        parser_read_byte(Serial.read());
    }
}
