#ifndef FUNCTION_PROTOTYPE_H
#define FUNCTION_PROTOTYPE_H

/*
 * Use function prototypes for built-in functions
 * to check arguments at compile time (asm_functions.c).
 * This is just to make things easier compared to
 * defining a full function.
 */

// forward declaration
enum token_type_e;

struct function_prototype_t
{
	enum token_type_e* parameter_types;
};
#endif
