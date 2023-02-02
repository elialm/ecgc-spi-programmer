#include "byte_regex.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

int byte_regex_compile_pattern(const char* pattern_in, uint8_t* pattern_out, unsigned int* pattern_length)
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
    unsigned int pattern_length = strlen(pattern);

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

int byte_regex_match(const uint8_t* pattern, const uint8_t* data)
{
    if (pattern == NULL || data == NULL) {
        return -EINVAL;
    }

    return 0;
}
