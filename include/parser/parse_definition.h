#ifndef PARSE_DEFINITION_H
#define PARSE_DEFINITION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "token_array.h"
#include "token.h"
#include "err_msg.h"

// used to parse variable/function defintions:
// EXAMPLES:
// int32 x; (auto-initializes to 0)
// int32 x = 5;
// int32 func();
// int32 func(int32 x, int32 y) {}
bool parse_definition(
		struct token_array_t* token_array,
		size_t* token_array_idx,
		struct token_array_t* token_buffer,
		struct err_msg_t* err);

#endif
