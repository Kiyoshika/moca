#include "asm_registers.h"
#include "asm_functions.h"
#include "asm_sections.h"
#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "parameters.h"
#include "err_msg.h"
#include "built_in_functions.h"
#include "function_prototype.h"
#include "util.h"

// check if assignment value (arg2) is a variable (if it's alphanumeric and contains at least one letter)
static bool _check_is_variable(const char* argvalue)
{
	bool contains_letter = false;
	for (size_t i = 0; i < strlen(argvalue); ++i)
	{
		if (isalpha(argvalue[i]))
			contains_letter = true;

		if (!isalnum(argvalue[i]))
			return false; // immediately fail if character is not alphanumeric
	}

	if (!contains_letter) // must have at least one letter
		return false;

	return true;
}

// get the total stack size of all parameters
// that we can use to offset for fetching local variables (non-parameters)
size_t _get_parameter_stack_size(
		const struct function_t* function)
{
	size_t parameter_stack_size = 0;
	for (size_t i = 0; i < function->n_parameters; ++i)
		parameter_stack_size += function->parameters[i].variable.bytes_size;

	return parameter_stack_size;
}

// find position of variable on the stack (in bytes)
// returns false  if not found, otherwise returns true
static bool _find_variable_position(
		const struct function_t* function,
		const char* variable_name,
		size_t* variable_idx,
		ssize_t* variable_stack_position,
		size_t* variable_bytes_size,
		bool* is_parameter)
{
	size_t initial_stack_position = *variable_stack_position;
	for (size_t i = 0; i < function->n_parameters; ++i)
	{
		*variable_stack_position += function->parameters[i].variable.bytes_size;
		if (strcmp(function->parameters[i].variable.name, variable_name) == 0)
		{
			*variable_bytes_size = function->parameters[i].variable.bytes_size;
			*variable_idx = i;
			*is_parameter = true;
			return true;
		}
	}
	// if not parameter, reset stack position
	*variable_stack_position = initial_stack_position;

	for (size_t i = 0; i < function->n_variables; ++i)
	{
		*variable_stack_position += function->variables[i].bytes_size;
		if (strcmp(function->variables[i].name, variable_name) == 0)
		{
			*variable_bytes_size = function->variables[i].bytes_size;
			*variable_idx = i;
			*is_parameter = false;
			return true;
		}
	}

	return false;
}

// check if variable is in global scope (currently only used for strings)
static bool _is_global_variable(
		const struct global_scope_t* global_scope,
		const char* variable_name,
		size_t* global_var_idx)
{
	for (size_t i = 0; i < global_scope->n_variables; ++i)
	{
		if (strcmp(global_scope->variables[i].name, variable_name) == 0)
		{
			*global_var_idx = i;
			return true;
		}
	}

	return false;
}

// TODO: move this into separate file asm_instructions.c
static void _get_move_instruction(char (*move_text)[5], size_t size)
{
	switch (size)
	{
		case 1:
			memcpy(move_text, "movb\0", 5);
			break;
		case 2:
			memcpy(move_text, "movw\0", 5);
			break;
		case 4:
			memcpy(move_text, "movl\0", 5);
			break;
		case 8:
			memcpy(move_text, "movq\0", 5);
			break;
		default:
			break;
	}
}

static bool _initialize_variable(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		const struct function_t* function,
		size_t function_idx,
		const size_t parameter_stack_size,
		struct err_msg_t* err)
{
	// TODO: this stack position does NOT currently
	// account for 7th+ parameters (which get allocated
	// on the stack)
	size_t variable_idx = 0;
	// some stack space can be occupied by parameters, we allocate after those
	ssize_t variable_stack_position = parameter_stack_size;
	size_t variable_bytes_size = 0;
	bool is_parameter = false;

	_find_variable_position(
			function,
			function->instruction_arg1[function_idx],
			&variable_idx,
			&variable_stack_position,
			&variable_bytes_size,
			&is_parameter);

	// string literals will be added to global scope
	if (function->variables[variable_idx].type == STRING)
	{
		gscope_add_variable(
			global_scope,
			&function->variables[variable_idx],
			err);
		return true;
	}

	char mov_text[5];
	_get_move_instruction(&mov_text, variable_bytes_size);

	// using rbx as a temp register
	char variable_register_text[5];
	asm_get_rbx_register(&variable_register_text, variable_bytes_size);

	// this is the stack position of the variable's value
	// we're assigning (e.g., int32 x = [y] <--).
	// We will need to move this into a temp register (say, %rbx)
	// before moving that into the appropriate variable_stack_position
	ssize_t variable_assign_stack_position = parameter_stack_size;
	size_t variable_assign_bytes_size = 0;

	if (_check_is_variable(
			function->instruction_arg1[function_idx]))
	{
		if (!_find_variable_position(
			function,
			function->instruction_arg1[function_idx],
			&variable_idx,
			&variable_assign_stack_position,
			&variable_assign_bytes_size,
			&is_parameter))
		{
			// TODO: add line & char information in
			// variable_t, function_t, and parameter_t
			err_write(err, "Unknown variable name.", 0, 0);
			return false;
		}

		char assign_mov_text[5];
		_get_move_instruction(&assign_mov_text, variable_assign_bytes_size);

		// using rbx as a temp register
		char variable_assign_register_text[5];
		asm_get_rbx_register(&variable_assign_register_text, variable_assign_bytes_size);

		// move assignment variable into temp register (i.e., int32 x = [y])
		// e.g., movq -12(%rbp), %rbx
		char rbp_register_text[5];
		asm_get_rbp_register(&rbp_register_text);

		fprintf(asm_file, "\t%s%s%zu(%s), %s\n",
				assign_mov_text, " -", variable_assign_stack_position,
				rbp_register_text, variable_assign_register_text);

		// move temp register into variable stack position
		// (the one we're assigning to, i.e., int32 [x] = y)
		// e.g., movq %rbx, -12(%rbp)
		// NOTE: we use the size of the left-hand variable, which may be larger
		// or smaller than the right-hand variable
		fprintf(asm_file, "\t%s%s%s%s%zu(%s)\n",
				mov_text, " ", variable_register_text, ", -", variable_stack_position, rbp_register_text);
	}
	else
	{
		char rbp_register_text[5];
		asm_get_rbp_register(&rbp_register_text);

		// move constant into stack
		// e.g., movq $12, -15(%rbp)
		fprintf(asm_file, "\t%s%s%s%s%s%zu(%s)\n", 
				mov_text, 
				" $", function->instruction_arg2[function_idx], ",",
				"-", variable_stack_position, rbp_register_text);
	}

	return true;
}

// get the appropriate register for the current argument (1-6).
// returns whether or not variable must be pushed onto the stack.
// if argument is in 7th position or beyond, return true as it will
// need to be pushed onto the stack
static bool _get_arg_register(
		char (*register_text)[5],
		size_t bytes_size,
		size_t function_call_arg_counter)
{
	switch (function_call_arg_counter)
	{
		case 0:
			asm_get_rdi_register(register_text, bytes_size);
			return false;
		case 1:
			asm_get_rsi_register(register_text, bytes_size);
			return false;
		case 2:
			asm_get_rdx_register(register_text, bytes_size);
			return false;
		case 3:
			asm_get_rcx_register(register_text, bytes_size);
			return false;
		case 4:
			asm_get_r8_register(register_text, bytes_size);
			return false;
		case 5:
			asm_get_r9_register(register_text, bytes_size);
			return false;
		default:
			return true;
	}

	return true;
}

static bool _set_arg_counter(
		struct global_scope_t* global_scope,
		struct function_t* function,
		const size_t instruction_idx,
		bool* using_builtin_function,
		struct function_t** reference_function,
		struct function_prototype_t** reference_built_in_function,
		size_t* function_call_arg_counter,
		struct err_msg_t* err)
{
	if (instruction_idx == 0 || function->instruction_code[instruction_idx-1] != ADD_ARG)
	{
		*reference_function = NULL;
		*function_call_arg_counter = 0;

		char function_name[FUNCTION_NAME_LEN];
		memcpy(function_name, function->instruction_arg1[instruction_idx], FUNCTION_NAME_LEN);

		for (size_t i = 0; i < global_scope->n_functions; ++i)
		{
			if (strcmp(function_name, global_scope->functions[i].name) == 0)
			{
				*reference_function = &global_scope->functions[i];
				*using_builtin_function = false;
				break;
			}
		}

		// if function not in global scope, check built-in functions
		if (!*reference_function)
		{
			const char* arg_value = function->instruction_arg2[instruction_idx];
			for (size_t i = 0; i < global_scope->n_built_in_functions; ++i)
			{
				if (strcmp(function_name, global_scope->built_in_functions[i].name) == 0)
				{
					*reference_built_in_function = &global_scope->built_in_functions[i];
					*using_builtin_function = true;

					// SPECIAL CASE: if printf, take this first argument (string) and parse any string formatting
					// to dynamically determine expected arguments
					if (strcmp(function_name, "printf") == 0)
						if (!built_in_functions_parse_printf_args(global_scope, arg_value, err))
							return false;

					break;
				}
			}
		}

		if (!*reference_function && !*using_builtin_function)
		{
			err_write(err, "Unknown function.", 0, 0);
			return false;
		}
	}

	return true;
}

enum token_type_e _get_variable_type(
		const struct function_t* function,
		const size_t stack_position)
{
	size_t _stack_pos = 0;
	for (size_t i = 0; i < function->n_parameters; ++i)
	{
		_stack_pos += function->parameters[i].variable.bytes_size;
		if (_stack_pos == stack_position)
			return function->parameters[i].variable.type;
	}

	for (size_t i = 0; i < function->n_variables; ++i)
	{
		_stack_pos += function->variables[i].bytes_size;
		if (_stack_pos == stack_position)
			return function->variables[i].type;
	}

	return NONE; // technically should be unreachable
				 // as the variable is guaranteed to exist
				 // by this point
}

static bool _validate_return_type(
		const struct function_t* function,
		enum token_type_e return_type,
		const char* return_value,
		bool is_variable,
		struct err_msg_t* err)
{
	if (is_variable && return_type != function->return_type)
	{
		err_write(err, "Returned value does not match function return type.", 0, 0);
		return false;
	}

	bool is_numeric_literal = return_value[0] == '"' ? false : true;
	if (!is_variable)
	{
		// TODO: can probably provide explicit checks like
		// if returning 10000 from a int8 function (which would overflow)
		switch (function->return_type)
		{
			case INT8:
			case INT16:
			case INT32:
			case INT64:
				if (!is_numeric_literal)
				{
					err_write(err, "Returned value does not match function return type.", 0, 0);
					return false;
				}
				break;

			case STRING:
				if (is_numeric_literal)
				{
					err_write(err, "Returned value does not match function return type.", 0, 0);
					return false;
				}
				break;

			default:
				break;
		}
	}

	return true;
}

static bool _validate_arg_type(
		const bool is_parameter,
		const bool using_builtin_function,
		struct function_t* function,
		struct function_t* reference_function,
		struct function_prototype_t* reference_built_in_function,
		const size_t function_call_arg_counter,
		const size_t variable_idx,
		struct err_msg_t* err)
{
	switch (is_parameter)
	{
		case false:
			// SPECIAL CASE: printf can take arbitrary integer types, e.g., '%d' format can be any of INT8, INT16, INT32 and INT64.
			// Printf uses 'INT32' internally but can be any of the above types mentioned
			if (using_builtin_function && strcmp(reference_built_in_function->name, "printf") == 0)
			{
				if (reference_built_in_function->parameter_types[function_call_arg_counter] == INT32)
				{
					switch (function->variables[variable_idx].type)
					{
						case INT8:
						case INT16:
						case INT32:
						case INT64:
							// valid
							break;
						default:
							err_write(err, "Passed incorrect type according to printf format.", 0, 0);
							return false;
					}
				}
			}
			else if ((!using_builtin_function && function->variables[variable_idx].type != reference_function->parameters[function_call_arg_counter].variable.type)
				|| (using_builtin_function && function->variables[variable_idx].type != reference_built_in_function->parameter_types[function_call_arg_counter]))
			{
				err_write(err, "Passed incorrect type to function.", 0, 0);
				return false;
			}
		break;

		case true:
			if ((!using_builtin_function && function->parameters[variable_idx].variable.type != reference_function->parameters[function_call_arg_counter].variable.type)
				|| (using_builtin_function && function->parameters[variable_idx].variable.type != reference_built_in_function->parameter_types[function_call_arg_counter]))
			{
				err_write(err, "Passed incorrect type to function.", 0, 0);
				return false;
			}
		break;
	}

	return true;
}

// adding a string literal (called from _add_literal_arg())
static bool _add_string_literal_arg(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		struct function_t* function,
		char* literal_value,
		const bool using_builtin_function,
		const size_t function_call_arg_counter,
		struct function_t* reference_function,
		struct function_prototype_t* reference_built_in_function,
		struct err_msg_t* err)
{
	if ((!using_builtin_function && reference_function->parameters[function_call_arg_counter].variable.type != STRING)
			|| (using_builtin_function && reference_built_in_function->parameter_types[function_call_arg_counter] != STRING))
	{
		err_write(err, "Tried to pass string to an integer parameter.", 0, 0);
		return false;
	}

	size_t global_var_idx = util_get_global_string_literal(
			global_scope,
			function,
			literal_value,
			err);

	char arg_register_text[5];
	memset(arg_register_text, 0, 5);

	bool is_stack = _get_arg_register(
			&arg_register_text,
			8, // pointers are 8 bytes
			function_call_arg_counter);

	if (is_stack)
	{
		printf("PUSH ARG ONTO STACK\n");
	}
	else
	{
		// load address into register
		// e.g., leaq name(%rip), %rdi
		fprintf(asm_file, "\tleaq %s(%%rip), %s\n",
			global_scope->variables[global_var_idx].name, arg_register_text);
	}

	return true;
}

// adding a numerical literal (called from _add_literal_arg())
static bool _add_numeric_literal_arg(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		const char* literal_value,
		const bool using_builtin_function,
		const size_t function_call_arg_counter,
		struct function_t* reference_function,
		struct function_prototype_t* reference_built_in_function,
		struct err_msg_t* err)
{
	// since it's a numerical literal, we don't know the size so we
	// lookup the expected size
	if ((!using_builtin_function && reference_function->parameters[function_call_arg_counter].variable.type == STRING)
		|| (using_builtin_function && reference_built_in_function->parameter_types[function_call_arg_counter] == STRING))
	{
		err_write(err, "Tried to pass integer to a string parameter.", 0, 0);
		return false;
	}

	size_t expected_size; 
	if (!using_builtin_function)
		expected_size = reference_function->parameters[function_call_arg_counter].variable.bytes_size;
	else
		expected_size = reference_built_in_function->parameter_byte_sizes[function_call_arg_counter];

	char arg_register_text[5];
	memset(arg_register_text, 0, 5);

	char mov_text[5];
	memset(mov_text, 0, 5);

	// move the literal into correct register
	// e.g., movq $12, %rdi
	bool is_stack = _get_arg_register(
			&arg_register_text,
			expected_size,
			function_call_arg_counter);

	if (is_stack)
	{
		// TODO: push variable onto stack
		printf("PUSH ARG ONTO STACK\n");
	}
	else
	{
		_get_move_instruction(&mov_text, expected_size);
		fprintf(asm_file, "\t%s $%s, %s\n",
				mov_text, literal_value, arg_register_text);
	}

	return true;
}

// argument passed to function is a literal value (numeric or string)
// e.g., 13 or "hey"
static bool _add_literal_arg(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		struct function_t* function,
		char* literal_value,
		const bool using_builtin_function,
		struct function_t* reference_function,
		struct function_prototype_t* reference_built_in_function,
		const size_t function_call_arg_counter,
		struct err_msg_t* err)
{
	// if string literal, check global scope if this string value already exists
	// (then we wouldn't need to allocate another one)
	if (literal_value[0] == '"')
	{
		if (!_add_string_literal_arg(
			asm_file,
			global_scope,
			function,
			literal_value,
			using_builtin_function,
			function_call_arg_counter,
			reference_function,
			reference_built_in_function,
			err))
			return false;
	}
	else
	{
		if (!_add_numeric_literal_arg(
			asm_file,
			global_scope,
			literal_value,
			using_builtin_function,
			function_call_arg_counter,
			reference_function,
			reference_built_in_function,
			err))
			return false;
	}

	return true;
}

// argument passed to function is a variable name format (e.g., myvar123)
// this will attempt to extract the value/size/position and move it into
// appropriate register or stack
static bool _add_variable_arg(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		struct function_t* function,
		const char* variable_name,
		bool using_builtin_function,
		struct function_t* reference_function,
		struct function_prototype_t* reference_built_in_function,
		const size_t function_call_arg_counter,
		struct err_msg_t* err)
{
	size_t variable_idx = 0;
	ssize_t variable_stack_position = 0;
	size_t variable_bytes_size = 0;
	bool is_parameter = false;

	char mov_text[5];
	char arg_register_text[5];
	memset(mov_text, 0, 5);
	memset(arg_register_text, 0, 5);

	if (_find_variable_position(
			function,
			variable_name,
			&variable_idx,
			&variable_stack_position,
			&variable_bytes_size,
			&is_parameter))
	{
		// validate expected type
		if (!_validate_arg_type(
			is_parameter,
			using_builtin_function,
			function,
			reference_function,
			reference_built_in_function,
			function_call_arg_counter,
			variable_idx,
			err))
			return false;

		// if variable is a STRING, then pass 8 bytes to get the largest register size (since
		// we actually pass the address of the string, which is 8 bytes)
		if ((is_parameter && function->parameters[variable_idx].variable.type == STRING)
			|| (!is_parameter && function->variables[variable_idx].type == STRING))
			variable_bytes_size = 8;

		// fetch variable value
		bool is_stack = _get_arg_register(
				&arg_register_text,
				variable_bytes_size,
				function_call_arg_counter);

		if (is_stack)
		{
			// TODO: push variable onto stack
			printf("PUSH ARG ONTO STACK\n");
		}
		else
		{
			size_t global_var_idx = 0;
			if (_is_global_variable( // check if it's a global variable
				global_scope,
				variable_name,
				&global_var_idx))
			{
				// if global varaible is a string, we'll use leaq instead of mov
				if (global_scope->variables[global_var_idx].type == STRING)
				{
					fprintf(asm_file, "\tleaq %s(%%rip), %s\n",
						global_scope->variables[global_var_idx].name, arg_register_text);
				}
				else
				{
					// TODO: handle global variables that are not strings
				}
			}
			else // otherwise it's a local variable (or parameter)
			{
				// move variable from stack into appropriate register
				// e.g., movq -12(%rsp), %rdi
				_get_move_instruction(&mov_text, variable_bytes_size);

				fprintf(asm_file, "\t%s -%zu(%%rsp), %s\n",
						mov_text, variable_stack_position, arg_register_text);
			}
		}
	}
	else
	{
		// TODO: add line & char position
		err_write(err, "Unknown variable name.", 0, 0);
		return false;
	}

	return true;
}

// add argument to function call (move into register or push onto stack)
static bool _add_argument(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		struct function_t* function,
		const size_t instruction_idx,
		const size_t parameter_stack_size,
		bool* using_builtin_function,
		size_t* function_call_arg_counter,
		struct function_t** reference_function,
		struct function_prototype_t** reference_built_in_function,
		struct err_msg_t* err)
{
	// to support multiple function calls, reset the current argument counter
	// prior to making a new function call (or increment counter if we are
	// passing another argument)
	if (!_set_arg_counter(
			global_scope,
			function,
			instruction_idx,
			using_builtin_function,
			reference_function,
			reference_built_in_function,
			function_call_arg_counter,
			err))
		return false;
		
	
	if ((!*using_builtin_function && *function_call_arg_counter + 1 > (*reference_function)->n_parameters)
		|| (*using_builtin_function && *function_call_arg_counter + 1 > (*reference_built_in_function)->n_parameters))
	{
		err_write(err, "Passed too many arguments to function.", 0, 0);
		return false;
	}

	/* check if argument is a variable name or literal value */

	// variable format (e.g., myvar123)
	if (_check_is_variable(
			function->instruction_arg2[instruction_idx]))
	{
		if (!_add_variable_arg(
			asm_file,
			global_scope,
			function,
			function->instruction_arg2[instruction_idx],
			*using_builtin_function,
			*reference_function,
			*reference_built_in_function,
			*function_call_arg_counter,
			err))
			return false;
	}
	// either numeric or string literal (e.g., 13 or "hey")
	else 
	{
		if (!_add_literal_arg(
			asm_file,
			global_scope,
			function,
			function->instruction_arg2[instruction_idx],
			*using_builtin_function,
			*reference_function,
			*reference_built_in_function,
			*function_call_arg_counter,
			err))
			return false;

	}

	// if end of instructions or next instruction is not a new argument, validate
	// that we passed the correct number of arguments
	if ((instruction_idx == function->n_instructions - 1 || function->instruction_code[instruction_idx + 1] != ADD_ARG)
		&& ((!*using_builtin_function && *function_call_arg_counter + 1 < (*reference_function)->n_parameters)
			|| (*using_builtin_function && *function_call_arg_counter + 1 < (*reference_built_in_function)->n_parameters)))
	{
		err_write(err, "Passed fewer than expected arguments to function.", 0, 0);
		return false;
	}

	(*function_call_arg_counter)++;

	return true;
}

static bool _return_function(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		const struct function_t* function,
		const char* variable_name,
		const char* return_value,
		struct err_msg_t* err)
{
	char mov_text[5];
	char return_register_text[5];

	if (_check_is_variable(variable_name))
	{
		size_t variable_idx = 0;
		ssize_t variable_stack_position = 0;
		size_t variable_bytes_size = 0;
		bool is_parameter = false;
		if (!_find_variable_position(
				function,
				variable_name,
				&variable_idx,
				&variable_stack_position,
				&variable_bytes_size,
				&is_parameter))
		{
			// TODO: add line & char information in
			// variable_t, function_t and parameter_t
			err_write(err, "Unknown variable name.", 0, 0);
			return false;
		}

		// for local variables, we want to offset the stack position
		// by the total stack size occupied by parameters
		if (!is_parameter)
			variable_stack_position += _get_parameter_stack_size(function);


		enum token_type_e return_type = _get_variable_type(function, variable_stack_position);
		if (!_validate_return_type(
				function, 
				return_type, 
				return_value,
				true,
				err))
			return false;

		_get_move_instruction(&mov_text, variable_bytes_size);
		asm_get_rax_register(&return_register_text, variable_bytes_size);

		if (return_type == STRING)
		{
			fprintf(asm_file, "\tleaq %s(%%rip), %%rax\n",
					return_value);
		}
		else
		{
			// move variable from stack into rax register to return from function
			fprintf(asm_file, "\t%s -%zu(%%rbp), %s\n",
				mov_text, variable_stack_position, return_register_text);
		}
	}
	else // numeric or string literal
	{
		if (!_validate_return_type(
				function,
				NONE,
				return_value,
				false,
				err))
			return false;

		size_t return_size = tkn_get_bytes_size(function->return_type);

		_get_move_instruction(&mov_text, return_size);
		asm_get_rax_register(&return_register_text, return_size);

		// TODO: add support for string literals
		if (return_value[0] == '"')
		{
			size_t global_var_idx = util_get_global_string_literal(
					global_scope,
					function,
					return_value,
					err);
			char var_name[VARIABLE_NAME_LEN];
			memset(var_name, 0, VARIABLE_NAME_LEN);
			const char* name = global_scope->variables[global_var_idx].name;
			size_t name_len = strlen(name);
			name_len = name_len > (size_t)VARIABLE_NAME_LEN - 1 ? (size_t)VARIABLE_NAME_LEN - 1 : name_len;
			memcpy(var_name, global_scope->variables[global_var_idx].name, name_len);
			var_name[name_len] = '\0';

			// strings will always use rax since they are the
			// largest size (8 bytes)
			fprintf(asm_file, "\tleaq %s(%%rip), %%rax\n",
					var_name);
		}
		else
		{
			fprintf(asm_file, "\t%s $%s, %s\n",
				mov_text, return_value, return_register_text);
		}

	}

	return true;
}

static bool _asm_function_write_instructions(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		struct function_t* function,
		const size_t parameter_stack_size,
		struct err_msg_t* err)
{
	// need to maintain state here so we can pass by address
	// for subsequent calls to ADD_ARG
	size_t function_call_arg_counter = 0;
	bool using_builtin_function = false;
	struct function_t* reference_function = NULL;
	struct function_prototype_t* reference_built_in_function = NULL;

	for (size_t i = 0; i < function->n_instructions; ++i)
	{
		switch (function->instruction_code[i])
		{
			case INIT_VAR: // initialize variable
			{
				if (!_initialize_variable(
						asm_file, 
						global_scope, 
						function, 
						i, 
						parameter_stack_size, 
						err))
					return false;
				break;
			}

			case ADD_ARG:
			{
				if (!_add_argument(
						asm_file,
						global_scope,
						function,
						i,
						parameter_stack_size,
						&using_builtin_function,
						&function_call_arg_counter,
						&reference_function,
						&reference_built_in_function,
						err))
					return false;
				break;
			}

			case CALL_FUNC:
			{
				// special case: if calling printf, need to reset lower 8 bits of rax register (al)
				if (strcmp("printf", function->instruction_arg1[i]) == 0)
					fprintf(asm_file, "\txor %%al, %%al\n");

				fprintf(asm_file, "\tcall %s\n", function->instruction_arg1[i]);
				break;
			}

			case RETURN_FUNC:
			{
				// TODO: change instruction_arg1[i] to be the variable name
				// TODO: change instruction_arg2[i] to be the variable value
				if (!_return_function(
						asm_file,
						global_scope,
						function,
						function->instruction_arg1[i],
						function->instruction_arg2[i],
						err))
					return false;
				break;
			}

			default:
				break;
		}
	}

	return true;
}

bool asm_function_create(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		struct err_msg_t* err)
{
	for (size_t i = 0; i < global_scope->n_functions; ++i)
	{
		// general boilerplate for creating a function
		fprintf(asm_file, "\n%s:\n", global_scope->functions[i].name);
		fprintf(asm_file, "\t%s\n", "pushq %rbp");
		fprintf(asm_file, "\t%s\n", "movq %rsp, %rbp");

		// move function parameters into stack
		struct function_t* function = &global_scope->functions[i];
		size_t current_parameter = 0;
		size_t registers_used = function->n_parameters > 6 ? 6 : function->n_parameters;
		size_t current_stack_position = 0;
		char register_text[5];
		char mov_text[5];
		memset(register_text, 0, 5);
		memset(mov_text, 0, 5);
		// first six parameters are in regsiters, remaining are on stack
		for (current_parameter = 0; current_parameter < registers_used; ++current_parameter)
		{
			const struct variable_t* param_variable = &function->parameters[current_parameter].variable;
			size_t param_size = 0;

			// TODO: we wouldn't need this condition if we forced strings to be 8 bytes (variables.c??)
			if (param_variable->type == STRING)
				param_size = 8; // strings will be passed by address, so 8 bytes
			else
				param_size = param_variable->bytes_size;

			_get_arg_register(&register_text, param_size, current_parameter);
			current_stack_position += param_size;
			_get_move_instruction(&mov_text, param_size);

			// e.g., movq %rdi, -4(%rbp)
			fprintf(asm_file, "\t%s %s, -%zu(%%rbp)\n",
				mov_text, register_text, current_stack_position);
		}
		if (function->n_parameters > 6)
		{
			// TODO: push remaining parameters right-to-left onto stack
			printf("TODO: push remaining parameters right-to-left onto stack.\n");
		}

		// translate function instructions into raw assembly
		if (!_asm_function_write_instructions(asm_file, global_scope, &global_scope->functions[i], current_stack_position, err))
			return false;

		// cleanup stack and restore base pointer
		fprintf(asm_file, "\t%s\n", "leave");

		// main function needs special return
		if (strcmp(global_scope->functions[i].name, "main") == 0)
		{
			fprintf(asm_file, "\t%s\n", "xor %rdi, %rdi");
			fprintf(asm_file, "\t%s\n", "call exit");
		}
		else
			fprintf(asm_file, "\t%s\n", "ret");
	}

	return true;
}
