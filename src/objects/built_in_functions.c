#include "built_in_functions.h"
#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "parameters.h"
#include "err_msg.h"
#include "function_prototype.h"

char* built_in_functions[N_BUILT_IN_FUNCTIONS] = {
	"printf"
};

static bool _add_arg_type(
	enum token_type_e** arg_types,
	enum token_type_e arg_type,
	size_t* n_arg_idx,
	size_t* n_arg_types,
	struct err_msg_t* err)
{
	(*arg_types)[(*n_arg_idx)++] = arg_type;
	if (*n_arg_idx == *n_arg_types)
	{
		void* alloc = realloc(*arg_types, *n_arg_types * 2 * sizeof(enum token_type_e));
		if (!alloc)
		{
			err_write(err, "Couldn't allocate memory when trying to parse printf args.\n", 0, 0);
			return false;
		}
		*arg_types = alloc;
		*n_arg_types *= 2;
	}

	return true;
}

bool built_in_functions_parse_printf_args(
	struct global_scope_t* global_scope,
	const char* formatted_string,
	struct err_msg_t* err)
{
	size_t n_arg_types = 6;
	size_t n_arg_idx = 0;
	enum token_type_e* arg_types = malloc(n_arg_types * sizeof(enum token_type_e));
	if (!arg_types)
	{
		err_write(err, "Couldn't allocate memory when trying to parse printf args.\n", 0, 0);
		return false;
	}

	// first argument should be a string no matter what
	arg_types[n_arg_idx++] = STRING;

	// printf is the first built-in function (index 0)
	struct function_prototype_t* _printf = &global_scope->built_in_functions[0];
	for (size_t i = 0; i < strlen(formatted_string); ++i)
	{
		if (i < strlen(formatted_string) - 1 && formatted_string[i] == '%')
		{
			switch (formatted_string[i + 1])
			{
				case '%': // ignore '%%'
					continue;
					break;

				case 's': // %s is STRING type
					if (!_add_arg_type(&arg_types, STRING, &n_arg_idx, &n_arg_types, err))
						return false;
					break;

				case 'd': // %d is any INTEGER type (INT8, INT16, etc.)
				{
					if (!_add_arg_type(&arg_types, INT32, &n_arg_idx, &n_arg_types, err))
						return false;
					break;
				}
			}
		}
	}

	func_proto_set_arg_types(_printf, arg_types, n_arg_idx);

	return true;
}

static bool _create_printf(
	struct global_scope_t* global_scope,
	struct err_msg_t* err)
{
	// initially printf will only take a STRING argument, but
	// will be parsed for different formats (%d, %s, etc.) which
	// can dynamically expand the parameter types
	struct function_prototype_t _printf;
	func_proto_init(&_printf);
	func_proto_set_name(&_printf, "printf");
	
	enum token_type_e printf_args[1] = { STRING };
	func_proto_set_arg_types(&_printf, printf_args, 1);

	if (!gscope_add_function_prototype(global_scope, &_printf, err))
	{
		func_proto_free(&_printf);
		return false;
	}

	return true;
}

bool built_in_functions_create(
	struct global_scope_t* global_scope,
	struct err_msg_t* err)
{
	if (!_create_printf(global_scope, err))
		return false;
	
	return true;
}
