#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdbool.h>
#include <string.h>
#include "variables.h"

struct parameter_t
{
	struct variable_t variable;
	bool is_reference; // &x vs x
};

// create new parameter from variable (contents are copied)
// by default, parameter is NOT a reference (pass by value)
void parameter_create(
		struct parameter_t* parameter,
		const struct variable_t* variable);

// set whether or not the parameter is a reference
void parameter_set_is_reference(
		struct parameter_t* parameter,
		const bool is_reference);
#endif
