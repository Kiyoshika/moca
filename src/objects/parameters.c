#include "parameters.h"
#include "variables.h"
void parameter_create(
		struct parameter_t* parameter,
		const struct variable_t* variable)
{
	memcpy(&parameter->variable, variable, sizeof(struct variable_t));
	parameter->is_reference = false;
}

void parameter_set_is_reference(
		struct parameter_t* parameter,
		const bool is_reference)
{
	parameter->is_reference = is_reference;
}
