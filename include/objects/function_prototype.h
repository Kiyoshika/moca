#ifndef FUNCTION_PROTOTYPE_H
#define FUNCTION_PROTOTYPE_H

/*
 * Use function prototypes for built-in functions
 * to check arguments at compile time (asm_functions.c).
 * This is just to make things easier compared to
 * defining a full function.
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FUNC_PROTO_NAME_LEN 51

// forward declaration
enum token_type_e;

struct function_prototype_t
{
	char name[FUNC_PROTO_NAME_LEN];
	enum token_type_e* parameter_types;
	size_t n_parameters;
};

/*
 * LIST OF BUILT-IN FUNCTIONS:
 * These will be defined in built_in_functions.c
 */
extern struct function_prototype_t _printf;

// initialize function prototype by setting its
// name to zero and NULL parameter_types.
void func_proto_init(
	struct function_prototype_t* function_prototype);

// set the name of the prototype function.
// capped at a maximum of 50 characters (will be truncated if any longer).
void func_proto_set_name(
	struct function_prototype_t* function_prototype,
	const char* name);

// set the expected types for a function.
// parameter_types will automatically be reallocated to the appropriate size.
// will return false if failed to allocate memory; true otherwise.
bool func_proto_set_arg_types(
	struct function_prototype_t* function_prototype,
	const enum token_type_e* parameter_types,
	const size_t n_parameters);

// free memory allocated to function_prototype and reset name/parameter count.
void func_proto_free(
	struct function_prototype_t* function_prototype);

#endif
