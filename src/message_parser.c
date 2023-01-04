#include "message_parser.h"

#include <stdbool.h>
#include <inttypes.h>
#include <stddef.h>
#include <string.h>

static char message[MAX_MESSAGE_SIZE + 1] = {0};
static uint8_t message_index = 0;
static bool hold = false;
static uint8_t command = 0;

static void read_message()
{
	message[message_index] = '\0';
	command = parser_read_cmd(message);
	
	message_index = 0;
	hold = false;
}

uint8_t parser_get_cmd()
{
	uint8_t cmd = command;
	command = 0;
	return cmd;
}

void parser_read_letter(const char letter)
{
	bool read_after_write = false;
	
	if (letter == MESSAGE_BEGIN)
	{
		message_index = 0;
		hold = true;
	}
	else if (letter == MESSAGE_END && hold)
	{
		read_after_write = true;
	}
	else if (hold)
	{
		if (message_index == (MAX_MESSAGE_SIZE - 2)) {
			read_after_write = true;
		}
		
		message[message_index++] = letter;
	}
	
	if (read_after_write) {
		read_message();
	}
}

int8_t parser_message_builder(char* output, const char* message)
{
	if (output == NULL || message == NULL) {
		return -1;
	}
	
	strcpy(output + 1, message);
	
	uint8_t str_length = strlen(output);
	
	output[0] = MESSAGE_BEGIN;
	output[str_length] = MESSAGE_END;
	output[str_length + 1] = '\0';
	
	return 0;
}