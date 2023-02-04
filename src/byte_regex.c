#include "byte_regex.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

unsigned int min_uint(unsigned int a, unsigned int b) {
    return a < b ? a : b;
}

int byte_regex_compile_pattern(const char* pattern_in, uint8_t* pattern_out, size_t* pattern_length)
{
    if (pattern_in == NULL || pattern_out == NULL || pattern_length == NULL) {
        return -EINVAL;
    }

    return 0;
}

int byte_regex_validate_pattern(const uint8_t* pattern)
{
    if (pattern == NULL) {
        return -EINVAL;
    }

    unsigned int i;
    unsigned int pattern_length = strlen((const char*)pattern);

    if (pattern_length == 0) {
        return -EINVAL;
    }

    for (i = 0; i < pattern_length; i++) {
        uint8_t b = pattern[i];
        uint8_t cmd = b & 0x0F;
        uint8_t arg = b & 0xF0;
        
        switch (cmd) {
            // expect next n bytes (given by arg)
            case 0x01:
                i += (arg >> 4) + 1;
                break;

            // expect next byte n times (given by arg)
            case 0x02:
                i++;
                break;

            // ignore next n bytes (given by b & 0xF0)
            case 0x0F:
                break;
            
            // unrecognised cmd: return index of error
            default:
                return i;
        }
    }

    // index must match pattern length (meaning loop ran well)
    return i == pattern_length ? 0 : -EINVAL;
}

int byte_regex_match(const uint8_t* pattern, const uint8_t* data, const size_t data_length)
{
    if (pattern == NULL || data == NULL) {
        return -EINVAL;
    }

    if (data_length == 0) {
        return -EINVAL;
    }

    unsigned int data_index = 0;
    unsigned int pattern_index = 0;
    bool error_occurred = false;
    bool pattern_ended = false;

    for (; !pattern_ended && data_index < data_length; pattern_index++) {
        uint8_t cmd = pattern[pattern_index] & 0x0F;
        uint8_t arg = pattern[pattern_index] & 0xF0;

        switch (cmd) {
            // null terminator: quit looping
            case 0x00:
                pattern_ended = true;
                break;

            // expect next n bytes (given by arg)
            case 0x01:
                arg = (arg >> 4) + 1;
                for (uint8_t i = 0; i < arg && data_index < data_length; i++, data_index++, pattern_index++) {
                    if (data[data_index] != pattern[pattern_index + 1]) {
                        error_occurred = true;
                        break;
                    }
                }
                break;

            // expect next byte n times (given by arg)
            case 0x02:
                arg = (arg >> 4) + 1;
                pattern_index++;
                for (uint8_t i = 0; i < arg && data_index < data_length; i++, data_index++) {
                    if (data[data_index] != pattern[pattern_index]) {
                        error_occurred = true;
                        break;
                    }
                }
                break;

            // ignore next n bytes (given by b & 0xF0)
            case 0x0F:
                data_index += min_uint((arg >> 4) + 1, data_length);
                if (data_index == data_length) {
                    error_occurred = true;
                }
                break;
            
            // unrecognised cmd
            default:
                error_occurred = true;
                break;
        }

        if (error_occurred) {
            break;
        }
    }

    // check if pattern ended on null terminator
    if (pattern[pattern_index] == '\0') {
        pattern_ended = true;
    }

    // if either data and pattern have not been exhausted, the pattern loop did not go successfully
    if (!pattern_ended || data_index != data_length) {
        error_occurred = true;
    }

    return error_occurred ? data_index + 1 : 0;
}
