#include "functions.h"
#include "variables.h"
#include "parameters.h"
#include "err_msg.h"

bool function_create(
		struct function_t* function,
		struct err_msg_t* err)
{
	memset(function->name, 0, FUNCTION_NAME_LEN);

	function->return_type = NONE;

	function->parameters_capacity = 6;
	function->parameters = malloc(function->parameters_capacity * sizeof(struct parameter_t));
	if (!function->parameters)
	{
		err_write(err, "Couldn't allocate memory for new function.", 0, 0);
		return false;
	}
	function->n_parameters = 0;

	function->variables_capacity = 6;
	function->variables = malloc(function->variables_capacity * sizeof(struct variable_t));
	if (!function->variables)
	{
		err_write(err, "Couldn't allocate memory for new function.", 0, 0);
		return false;
	}
	function->n_variables = 0;

	function->is_defined = false;

	return true;
}

bool function_set_return_type(
		struct function_t* function,
		enum token_type_e return_type,
		struct err_msg_t* err)
{
	switch (return_type)
	{
		case INT8:
		case INT16:
		case INT32:
		case INT64:
		{
			function->return_type = return_type;
			return true;
		}

		default:
		{
			err_write(err, "Cannot use this data type as a return value.", 0, 0);
			return false;
		}
	}

	return false;
}

bool function_set_name(
		struct function_t* function,
		const char* name,
		struct err_msg_t* err)
{
	if (strlen(name) > 50)
	{
		err_write(err, "Function name cannot be more than 50 characters.", 0, 0);
		return false;
	}

	memcpy(function->name, name, strlen(name));
	function->name[strlen(name)] = '\0';

	return true;
}

bool function_add_parameter(
		struct function_t* function,
		const struct parameter_t* parameter,
		struct err_msg_t* err)
{
	memcpy(&function->parameters[function->n_parameters++], parameter, sizeof(struct parameter_t));

	if (function->n_parameters == function->parameters_capacity)
	{
		void* alloc = realloc(function->parameters, function->parameters_capacity * 2 * sizeof(struct parameter_t));
		if (!alloc)
		{
			err_write(err, "Couldn't allocate memory while adding parameters to function.", 0, 0);
			return false;
		}
		function->parameters = alloc;
		function->parameters_capacity *= 2;
	}

	return true;
}

bool function_add_variable(
		struct function_t* function,
		const struct variable_t* variable,
		struct err_msg_t* err)
{
	memcpy(function->variables, variable, sizeof(struct variable_t));
	function->n_variables++;

	if (function->n_variables == function->variables_capacity)
	{
		void* alloc = realloc(function->variables, function->variables_capacity * 2 * sizeof(struct variable_t));
		if (!alloc)
		{
			err_write(err, "Couldn't allocate memory while adding variables to function.", 0, 0);
			return false;
		}
		function->variables = alloc;
		function->variables_capacity *= 2;
	}

	return true;
}

void function_free(
		struct function_t* function)
{
	free(function->parameters);
	function->parameters = NULL;
	function->n_parameters = 0;
	function->parameters_capacity = 0;

	free(function->variables);
	function->variables = NULL;
	function->n_variables = 0;
	function->variables_capacity = 0;

	function->is_defined = false;
}
