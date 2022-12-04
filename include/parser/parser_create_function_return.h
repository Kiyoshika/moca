#ifndef PARSER_CREATE_FUNCTION_RETURN
#define PARSER_CREATE_FUNCTION_RETURN

#include <stdbool.h>
#include <stddef.h>

// forward declarations
struct global_scope_t;
struct function_t;
struct token_array_t;
struct err_msg_t;

// parse function return statements
bool parser_create_function_return(
		struct global_scope_t* global_scope,
		struct function_t* function,
		struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		struct token_array_t* function_buffer,
		struct err_msg_t* err_msg);
#endif
