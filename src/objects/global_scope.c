#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "err_msg.h"

bool gscope_create(
		struct global_scope_t* global_scope,
		struct err_msg_t* err)
{
	// start with initial 10 functions/variables as a generous buffer
	// before performing reallocations
	global_scope->functions = calloc(10, sizeof(struct function_t));
	if (!global_scope->functions)
	{
		err_write(err, "Couldn't allocate memory for global scope.", 0, 0);
		return false;
	}
	global_scope->n_functions = 0;
	global_scope->function_capacity = 10;

	global_scope->variables = calloc(10, sizeof(struct variable_t));
	if (!global_scope->variables)
	{
		err_write(err, "Couldn't allocate memory for global scope.", 0, 0);
		return false;
	}
	global_scope->n_variables = 0;
	global_scope->variable_capacity = 10;

	return true;
}

bool gscope_add_function(
		struct global_scope_t* global_scope,
		const struct function_t* function,
		struct err_msg_t* err)
{
	memcpy(
			&global_scope->functions[global_scope->n_functions++],
			function,
			sizeof(struct function_t));

	if (global_scope->n_functions == global_scope->function_capacity)
	{
		void* alloc = realloc(
				global_scope->functions,
				global_scope->function_capacity * 2 * sizeof(struct function_t));

		if (!alloc)
		{
			err_write(err, "Couldn't reallocate memory for global scope.", 0, 0);
			return false;
		}

		global_scope->functions = alloc;
		global_scope->function_capacity *= 2;
	}

	return true;
}

bool gscope_add_variable(
		struct global_scope_t* global_scope,
		const struct variable_t* variable,
		struct err_msg_t* err)
{
	memcpy(
			&global_scope->variables[global_scope->n_variables++],
			variable,
			sizeof(struct variable_t));

	if (global_scope->n_variables == global_scope->variable_capacity)
	{
		void* alloc = realloc(
				global_scope->variables,
				global_scope->variable_capacity * 2 * sizeof(struct variable_t));

		if (!alloc)
		{
			err_write(err, "Couldn't reallocate memory for global scope.", 0, 0);
			return false;
		}

		global_scope->variables = alloc;
		global_scope->variable_capacity *= 2;
	}

	return true;
}

void gscope_free(
		struct global_scope_t* global_scope)
{
	free(global_scope->functions);
	global_scope->functions = NULL;

	free(global_scope->variables);
	global_scope->variables = NULL;
}