#include <stdint.h>

#ifndef MESSAGE_PARSER_H_
#define MESSAGE_PARSER_H_

// define MAX_MESSAGE_SIZE as default 32 bytes
#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE 32
#endif

// if MAX_MESSAGE_SIZE is bigger then 256, redefine as 255
#if MAX_MESSAGE_SIZE > 256
#undef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE 256
#endif

// if MESSAGE_END not defined, define MESSAGE_END as '\n'
#ifndef MESSAGE_END
#define MESSAGE_END ('\n')
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*parser_func_overload_handler_t)();
typedef void (*parser_func_message_handler_t)(const char*);

void parser_set_overload_handler(parser_func_overload_handler_t overload_handler);
void parser_set_message_handler(parser_func_message_handler_t message_handler);
void parser_read_byte(const char read_byte);

#ifdef __cplusplus
}
#endif

#endif /* MESSAGE_PARSER_H_ */