#include <stdint.h>

#ifndef MESSAGE_PARSER_H_
#define MESSAGE_PARSER_H_

// define MAX_MESSAGE_SIZE as default 32bytes
#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE 32
#endif

// if MAX_MESSAGE_SIZE is bigger then 255, redefine as 255
#if MAX_MESSAGE_SIZE > 255
#undef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE 255
#endif

// if MESSAGE_BEGIN not defined, define MESSAGE_BEGIN as '#'
#ifndef MESSAGE_BEGIN
#define MESSAGE_BEGIN ('#')
#endif

// if MESSAGE_END not defined, define MESSAGE_END as ';'
#ifndef MESSAGE_END
#define MESSAGE_END (';')
#endif

#ifdef __cplusplus
extern "C" {
#endif

void parser_read_letter(const char letter);

uint8_t parser_read_cmd(const char* message);
uint8_t parser_get_cmd();
int8_t parser_message_builder(char* output, const char* message);

#ifdef __cplusplus
}
#endif

#endif /* MESSAGE_PARSER_H_ */