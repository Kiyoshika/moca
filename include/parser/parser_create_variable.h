#ifndef PARSER_CREATE_VARIABLE_H
#define PARSER_CREATE_VARIABLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// forward declarations
struct token_array_t;
struct err_msg_t;
struct global_scope_t;
struct function_t;

// take token buffer representing the definition of a variable and
// add it to the global scope
// returns true on success, returns false otherwise and writes to [err].
bool parser_create_global_variable(
		struct global_scope_t* global_scope,
		struct token_array_t* token_buffer,
		struct err_msg_t* err);

// take a token buffer representing the definition of a variable and
// add it to the function's local scope.
// returns true on success, returns false otherwise and writes to [err].
bool parser_create_local_variable(
		struct function_t* function_scope,
		struct token_array_t* token_buffer,
		struct err_msg_t* err);

#endif
