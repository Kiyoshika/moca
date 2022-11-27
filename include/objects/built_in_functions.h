#ifndef BUILT_IN_FUNCTIONS_H
#define BUILT_IN_FUNCTIONS_H

#define N_BUILT_IN_FUNCTIONS 1

#include <stdbool.h>

// mainly legacy, will have to remove this later (all functions that use this
// will have to search global_scope->built_in_functions instead)
extern char* built_in_functions[N_BUILT_IN_FUNCTIONS];

// forward declaration
struct global_scope_t;
struct err_msg_t;

// add all the built-in functions to the global scope
bool built_in_functions_create(
	struct global_scope_t* global_scope,
	struct err_msg_t* err);

#endif
