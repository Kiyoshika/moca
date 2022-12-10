#ifndef PARSER_VARIABLE_ASSIGNMENT
#define PARSER_VARIABLE_ASSIGNMENT

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

// forward declarations
struct global_scope_t;
struct function_t;
struct token_array_t;
struct err_msg_t;

bool parser_variable_assignment(
		struct global_scope_t* global_scope,
		struct function_t* function,
		struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		struct token_array_t* function_buffer,
		struct err_msg_t* err);

#endif
