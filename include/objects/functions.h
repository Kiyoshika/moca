#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "token.h"

#define FUNCTION_NAME_LEN 51

// forward declarations
struct variable_t;
struct parameter_t;
struct err_msg_t;

enum instruction_code_e
{
	INIT_VAR // intialize variable
};

struct function_t
{
	char name[FUNCTION_NAME_LEN];

	enum token_type_e return_type;

	struct parameter_t* parameters; // function parameters
	size_t n_parameters;
	size_t parameters_capacity;

	struct variable_t* variables; // local variables in function stack
	size_t n_variables;
	size_t variables_capacity;

	bool is_defined; // if function is defined or only declared

	// handing instructions to translate to assembly
	size_t n_instructions;
	size_t instruction_capacity;
	enum instruction_code_e* instruction_code;
	char (*instruction_arg1)[51];
	char (*instruction_arg2)[51];
};

// pass a stack-allocated function_t to initalize its members.
// returns true on success, otherwise returns false and populates [err].
bool function_create(
		struct function_t* function,
		struct err_msg_t* err);

// set the return data type of the function.
// returns true on success, false if [return_type] is not one of the expected types and writes to [err]
bool function_set_return_type(
		struct function_t* function,
		enum token_type_e return_type,
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

// write an instruction to the function which will be translated into assembly
bool function_write_instruction(
		struct function_t* function,
		const enum instruction_code_e instruction_code,
		const char* instruction_arg1,
		const char* instruction_arg2,
		struct err_msg_t* err);

void function_free(
		struct function_t* function);

#endif
