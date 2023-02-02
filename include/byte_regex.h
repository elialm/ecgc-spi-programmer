#ifndef __BYTE_REGEX_H__
#define __BYTE_REGEX_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

int byte_regex_compile_pattern(const char* pattern_in, uint8_t* pattern_out, unsigned int* pattern_length);
int byte_regex_validate_pattern(const uint8_t* pattern);
int byte_regex_match(const uint8_t* pattern, const uint8_t* data);

#ifdef __cplusplus
}
#endif

#endif //__BYTE_REGEX_H__