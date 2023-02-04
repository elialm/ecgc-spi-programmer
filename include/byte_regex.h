#ifndef __BYTE_REGEX_H__
#define __BYTE_REGEX_H__

#include <inttypes.h>

#ifndef ENVIRONMENT_NATIVE
typedef unsigned int size_t;
#endif //ENVIRONMENT_NATIVE

#ifdef __cplusplus
extern "C" {
#endif

int byte_regex_compile_pattern(const char* pattern_in, uint8_t* pattern_out, size_t* pattern_length);
int byte_regex_validate_pattern(const uint8_t* pattern);
int byte_regex_match(const uint8_t* pattern, const uint8_t* data, const size_t data_length);

#ifdef __cplusplus
}
#endif

#endif //__BYTE_REGEX_H__