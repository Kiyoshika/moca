#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdbool.h>

// forward declaration
struct variable_t;

struct parameter_t
{
	struct variable_t variable;
	bool is_reference; // &x vs x
};

#endif
