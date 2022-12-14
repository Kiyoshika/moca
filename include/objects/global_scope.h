#ifndef GLOBAL_SCOPE_H
#define GLOBAL_SCOPE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

// forward declarations
struct function_t;
struct function_prototype_t;
struct variable_t;
struct err_msg_t;

// the global scope context of the entire program
struct global_scope_t
{
	struct function_t* functions; // all functions live in global scope
	size_t n_functions;
	size_t function_capacity;

	struct function_prototype_t* built_in_functions; // built in functions like printf, malloc, etc.
	size_t n_built_in_functions;
	size_t built_in_function_capacity;

	struct variable_t* variables; // global varaibles defined outside functions
	size_t n_variables;
	size_t variable_capacity;
};

// attempt to create the global scope context for the application to
// initialize its members.
// returns true on success, returns false otherwise and writes to [err].
bool gscope_create(
		struct global_scope_t* global_scope,
		struct err_msg_t* err);

// attempt to add a function to the global scope.
// the contents of [function] are copied.
// returns true on success, returns false otherwise and writes to [err].
bool gscope_add_function(
		struct global_scope_t* global_scope,
		const struct function_t* function,
		struct err_msg_t* err);

// attempt to add a function prototype to the global scope.
// these are mainly used for "defining" built-in function types
// to check parameters at compile time.
bool gscope_add_function_prototype(
	struct global_scope_t* global_scope,
	const struct function_prototype_t* function_prototype,
	struct err_msg_t* err);

// edit an existing function prototype in global scope
// by passing another function_prototype with the same name.
// e.g., if printf changes its parameters
bool gscope_edit_function_prototype(
	struct global_scope_t* global_scope,
	const struct function_prototype_t* function_prototype,
	struct err_msg_t* err);

// attempt to add a variable to the global scope.
// the contents of [variable] are copied.
bool gscope_add_variable(
		struct global_scope_t* global_scope,
		const struct variable_t* variable,
		struct err_msg_t* err);

// deallocate memory held by global scope
void gscope_free(
		struct global_scope_t* global_scope);

#endif
