#include "err_msg.h"

void err_write(
		struct err_msg_t* err,
		const char* msg,
		const size_t line_num,
		const size_t char_pos)
{
	err->line_num = line_num;
	err->char_pos = char_pos;

	// cap msg length at 500 chars
	size_t msg_len = strlen(msg);
	msg_len = msg_len > 500 ? 500 : msg_len;
	memcpy(err->msg, msg, msg_len);
	err->msg[msg_len] = '\0';
}

void err_print(
		const struct err_msg_t* err)
{
	// TODO: display +/- 5 lines of original source code
	printf("LINE: %zu\n", err->line_num);
	printf("POS: %zu\n", err->char_pos);
	printf("ERR: %s\n", err->msg);
}
