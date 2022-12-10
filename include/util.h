#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

// forward declarations
struct err_msg_t;
struct token_array_t;
struct global_scope_t;
struct function_t;
enum token_type_e;

// when parsing a token buffer, extract all the tokens
// between quote tokens
char* util_get_text_between_quotes(
	struct token_array_t* token_buffer,
	size_t* buffer_idx,
	struct err_msg_t* err);

// search the global scope for a string literal
// and return the index. if not found, it will be
// automatically added to the global scope
size_t util_get_global_string_literal(
		struct global_scope_t* global_scope,
		const struct function_t* function,
		const char* string_literal_value,
		struct err_msg_t* err);

// parse a variable/function name until a specified
// set of terminating tokens (i.e., to stop parsing).
// pass a [name_buffer] which is where the variable/function
// name will be written (target destination) as well as the
// maximum allowed length [max_name_len].
// returns true on success, false otherwise.
bool util_get_name_from_buffer(
		char* name_buffer,
		const struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		const enum token_type_e* terminating_tokens,
		const size_t n_terminating_tokens,
		const size_t max_name_len,
		struct err_msg_t* err);

#endif
