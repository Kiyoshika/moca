#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

// forward declarations
struct err_msg_t;
struct token_array_t;

// when parsing a token buffer, extract all the tokens
// between quote tokens
char* util_get_text_between_quotes(
	struct token_array_t* token_buffer,
	size_t* buffer_idx,
	struct err_msg_t* err);

#endif
