#include <stddef.h>
#include <stdbool.h>

// forward declarations
struct token_array_t;
struct err_msg_t;
struct global_scope_t;
struct function_t;

// take a token buffer representing the definition of a function
// and add it to global scope.
// returns true on success, returns false otherwise and writes to [err].
bool parser_create_function(
		struct global_scope_t* global_scope,
		struct token_array_t* token_buffer,
		struct err_msg_t* err);
