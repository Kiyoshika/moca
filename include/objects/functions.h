#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "token.h"

// forward declarations
struct variable_t;
struct parameter_t;
struct err_msg_t;

struct function_t
{
	char name[51];

	enum token_type_e return_type;

	struct parameter_t* parameters; // function parameters
	size_t n_parameters;

	struct variable_t* variables; // local variables in function stack
	size_t n_variables;
};

// pass a stack-allocated function_t to initalize its members.
// returns true on success, otherwise returns false and populates [err].
bool function_create(
		struct function_t* function,
		struct err_msg_t* err);

// attempt to set the function name (must be at most 50 chars).
// returns true on success, otherwise returns false and populates [err].
bool function_set_name(
		struct function_t* function,
		const char* name,
		struct err_msg_t* err);

// attempt to add a new parameter to function (contents of parameter will be copied).
// returns true on success, otherwise returns false and populates [err].
bool function_add_parameter(
		struct function_t* function,
		const struct parameter_t* parameter,
		struct err_msg_t* err);

// attempt to add a new variable to function's stack (contents of variable will be copied).
// returns true on success, otherwise returns false and populates [err].
bool function_add_variable(
		struct function_t* function,
		const struct variable_t* variable,
		struct err_msg_t* err);

#endif