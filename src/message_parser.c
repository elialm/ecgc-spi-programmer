#include "message_parser.h"

#include <stdbool.h>
#include <stddef.h>

static parser_func_overload_handler_t parser_overload_handler = NULL;
static parser_func_message_handler_t parser_message_handler = NULL;

static char msg_buffer[MAX_MESSAGE_SIZE + 1];
static uint8_t msg_index = 0;
static bool msg_overloaded = false;

void parser_set_overload_handler(parser_func_overload_handler_t overload_handler)
{
	parser_overload_handler = overload_handler;
}

void parser_set_message_handler(parser_func_message_handler_t message_handler)
{
	parser_message_handler = message_handler;
}

void parser_read_byte(const char read_byte)
{
	msg_buffer[msg_index] = read_byte;

	if (msg_overloaded) {
		if (read_byte == '\n') {
			msg_overloaded = false;
		}
	} else {
		if (read_byte == '\n') {
			msg_buffer[msg_index] = '\0';
			msg_index = 0;

			if (parser_message_handler) {
				parser_message_handler(msg_buffer);
			}
		} else {
			msg_index++;
			if (msg_index == MAX_MESSAGE_SIZE) {
				msg_overloaded = true;

				if (parser_overload_handler) {
					parser_overload_handler();
				}
			}
		}
	}
}