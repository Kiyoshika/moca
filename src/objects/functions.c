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

	function->n_instructions = 0;
	function->instruction_capacity = 10;

	function->instruction_code = malloc(function->instruction_capacity * sizeof(enum instruction_code_e));
	if (!function->instruction_code)
	{
		err_write(err, "Couldn't allocate memory for new function.", 0, 0);
		return false;
	}

	function->instruction_arg1 = malloc(function->instruction_capacity * sizeof(*function->instruction_arg1));
	if (!function->instruction_arg1)
	{
		err_write(err, "Couldn't allocate memory for new function.", 0, 0);
		return false;
	}

	function->instruction_arg2 = malloc(function->instruction_capacity * sizeof(*function->instruction_arg2));
	if (!function->instruction_arg2)
	{
		err_write(err, "Couldn't allocate memory for new function.", 0, 0);
		return false;
	}

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
	memcpy(&function->variables[function->n_variables++], variable, sizeof(struct variable_t));

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

bool function_write_instruction(
		struct function_t* function,
		const enum instruction_code_e instruction_code,
		const char* instruction_arg1,
		const char* instruction_arg2,
		struct err_msg_t* err)
{
	function->instruction_code[function->n_instructions] = instruction_code;

	size_t len = strlen(instruction_arg1);
	size_t write_len = len > 50 ? 50 : len;
	memcpy(function->instruction_arg1[function->n_instructions], instruction_arg1, write_len);
	function->instruction_arg1[function->n_instructions][write_len] = '\0';

	len = strlen(instruction_arg2);
	write_len = len > 50 ? 50 : len;
	memcpy(function->instruction_arg2[function->n_instructions], instruction_arg2, write_len);
	function->instruction_arg2[function->n_instructions][write_len] = '\0';

	function->n_instructions++;

	if (function->n_instructions == function->instruction_capacity)
	{
		void* alloc = realloc(function->instruction_code, function->instruction_capacity * 2 * sizeof(enum instruction_code_e));
		if (!alloc)
		{
			err_write(err, "Couldn't allocate more memory for function.", 0, 0);
			return false;
		}
		function->instruction_code = alloc;

		alloc = realloc(function->instruction_arg1, function->instruction_capacity * 2 * sizeof(*function->instruction_arg1));
		if (!alloc)
		{
			err_write(err, "Couldn't allocate more memory for function.", 0, 0);
			return false;
		}
		function->instruction_arg1 = alloc;

		alloc = realloc(function->instruction_arg2, function->instruction_capacity * 2 * sizeof(*function->instruction_arg2));
		if (!alloc)
		{
			err_write(err, "Couldn't allocate more memory for function.", 0, 0);
			return false;
		}
		function->instruction_arg2 = alloc;

		function->instruction_capacity *= 2;
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

	free(function->instruction_code);
	function->instruction_code = NULL;

	free(function->instruction_arg1);
	function->instruction_arg1 = NULL;

	free(function->instruction_arg2);
	function->instruction_arg2 = NULL;
}
