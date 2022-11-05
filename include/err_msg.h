#ifndef ERR_MSG_H
#define ERR_MSG_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

struct err_msg_t
{
	size_t line_num;
	size_t char_pos;
	char msg[501];
	size_t msg_len;
};

void err_write(
		struct err_msg_t* err,
		const char* msg,
		const size_t line_num,
		const size_t char_pos);

void err_print(
		const struct err_msg_t* err);
#endif
