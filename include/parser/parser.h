#ifndef PARSER_H
#define PARSER_H

// forward declaration
struct token_array_t;
struct err_msg_t;
struct global_scope_t;

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum parse_definition_type
{
	VARIABLE,
	FUNCTION
};

// attempt to parser tokens, returns true on success,
// otherwise return false and write to err_msg_t on parse failure.
bool parse_tokens(
	struct token_array_t* token_array,
	struct global_scope_t* global_scope,
	struct err_msg_t* err);

#endif
