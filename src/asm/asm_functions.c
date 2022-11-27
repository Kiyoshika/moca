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

// find position of variable on the stack (in bytes)
// returns false  if not found, otherwise returns true
static bool _find_variable_position(
		const struct function_t* function,
		const char* variable_name,
		size_t* variable_idx,
		ssize_t* variable_stack_position,
		size_t* variable_bytes_size)
{
	for (size_t i = 0; i < function->n_variables; ++i)
	{
		*variable_stack_position += function->variables[i].bytes_size;
		if (strcmp(function->variables[i].name, variable_name) == 0)
		{
			*variable_bytes_size = function->variables[i].bytes_size;
			*variable_idx = i;
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
		struct err_msg_t* err)
{
	// TODO: this stack position does NOT currently
	// account for 7th+ parameters (which get allocated
	// on the stack)
	size_t variable_idx = 0;
	ssize_t variable_stack_position = 0;
	size_t variable_bytes_size = 0;

	_find_variable_position(
			function,
			function->instruction_arg1[function_idx],
			&variable_idx,
			&variable_stack_position,
			&variable_bytes_size);

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
	ssize_t variable_assign_stack_position = 0;
	size_t variable_assign_bytes_size = 0;

	if (_check_is_variable(
			function->instruction_arg2[function_idx]))
	{
		if (!_find_variable_position(
			function,
			function->instruction_arg2[function_idx],
			&variable_idx,
			&variable_assign_stack_position,
			&variable_assign_bytes_size))
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

static bool _asm_function_write_instructions(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		const struct function_t* function,
		struct err_msg_t* err)
{
	size_t function_call_arg_counter = 0;
	struct function_t* reference_function = NULL;
	bool using_builtin_function = false;
	struct function_prototype_t* reference_built_in_function = NULL;

	for (size_t i = 0; i < function->n_instructions; ++i)
	{
		switch (function->instruction_code[i])
		{
			case INIT_VAR: // initialize variable
			{
				if (_initialize_variable(asm_file, global_scope, function, i, err))
					continue;
				else
					return false;
				break;
			}

			case ADD_ARG:
			{
				
				// to support multiple function calls, reset the current argument counter
				// prior to making a new function call
				if (i == 0 || function->instruction_code[i-1] != ADD_ARG)
				{
					reference_function = NULL;
					function_call_arg_counter = 0;

					char function_name[FUNCTION_NAME_LEN];
					memcpy(function_name, function->instruction_arg1[i], FUNCTION_NAME_LEN);

					for (size_t i = 0; i < global_scope->n_functions; ++i)
					{
						if (strcmp(function_name, global_scope->functions[i].name) == 0)
						{
							reference_function = &global_scope->functions[i];
							break;
						}
					}

					// if function not in global scope, check built-in functions
					if (!reference_function)
					{
						for (size_t i = 0; i < global_scope->n_built_in_functions; ++i)
						{
							if (strcmp(function_name, global_scope->built_in_functions[i].name) == 0)
							{
								reference_built_in_function = &global_scope->built_in_functions[i];
								using_builtin_function = true;
								break;
							}
						}
					}

					if (!reference_function && !using_builtin_function)
					{
						err_write(err, "Unknown function.", 0, 0);
						return false;
					}
				}
				else
					function_call_arg_counter++;
				
				
				if ((!using_builtin_function && function_call_arg_counter + 1 > reference_function->n_parameters)
					|| (using_builtin_function && function_call_arg_counter + 1 > reference_built_in_function->n_parameters))
				{
					err_write(err, "Passed too many arguments to function.", 0, 0);
					return false;
				}

				// check if argument is a variable name
				size_t variable_idx = 0;
				ssize_t variable_stack_position = 0;
				size_t variable_bytes_size = 0;
				char arg_register_text[5];
				char mov_text[5];

				if (_check_is_variable(
						function->instruction_arg2[i]))
				{
					if (_find_variable_position(
						function,
						function->instruction_arg2[i],
						&variable_idx,
						&variable_stack_position,
						&variable_bytes_size))
					{
						// if variable is a STRING, then pass 8 bytes to get the largest register size (since
						// we actually pass the address of the string, which is 8 bytes)
						if (function->variables[variable_idx].type == STRING)
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
							if (_is_global_variable(
								global_scope,
								function->instruction_arg2[i],
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
							else
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
					
				}
				else // either numeric or string literal
				{
					// if string literal, check global scope if this string value already exists
					// (then we wouldn't need to allocate another one)
					if (function->instruction_arg2[i][0] == '"')
					{
						if ((!using_builtin_function && reference_function->parameters[function_call_arg_counter].variable.type != STRING)
							|| (using_builtin_function && reference_built_in_function->parameter_types[function_call_arg_counter] != STRING))
						{
							err_write(err, "Tried to pass string to an integer parameter.", 0, 0);
							return false;
						}

						bool needs_allocation = true;
						size_t global_var_idx = 0;
						for (size_t i = 0; i < global_scope->n_variables; ++i)
						{
							if (strcmp(function->instruction_arg2[i], global_scope->variables[i].value) == 0)
							{
								needs_allocation = false;
								global_var_idx = i;
								break;
							}
						}

						if (needs_allocation)
						{
							// add to global scope with a name created from
							// the current function name and global scope idx
							char name[VARIABLE_NAME_LEN];
							memset(name, 0, VARIABLE_NAME_LEN);
							memcpy(name, function->name, 47); // use up to 47 chars of function name
							char inttostr[100]; // maximum of 999 global variables supported
							sprintf(inttostr, "%zu", global_scope->n_variables);
							strncat(name, inttostr, 3);

							struct variable_t new_global;
							variable_create(&new_global);

							variable_set_type(&new_global, STRING, err);
							variable_set_name(&new_global, name, err);
							variable_set_value(&new_global, function->instruction_arg2[i], err);
							variable_set_initialized(&new_global, true);

							if (!gscope_add_variable(global_scope, &new_global, err))
								return false;

							global_var_idx = global_scope->n_variables - 1;
						}

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
					}
					else
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
									mov_text, function->instruction_arg2[i], arg_register_text);
						}
					}
				}



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

		// translate function instructions into raw assembly
		if (!_asm_function_write_instructions(asm_file, global_scope, &global_scope->functions[i], err))
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
