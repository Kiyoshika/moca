#include "asm_functions.h"
#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "parameters.h"

void asm_create_text_section(FILE* asm_file)
{
	fprintf(asm_file, "%s\n", ".section .text");
	fprintf(asm_file, "%s\n", ".globl main");
}

// this computes the total stack space used in a function
static void _asm_function_allocate_stack(
		FILE* asm_file,
		const struct function_t* function,
		size_t* stack_bytes_size)
{
	if (function->n_parameters > 6)
		for (size_t i = 7; i < function->n_parameters; ++i)
			*stack_bytes_size += function->parameters[i].variable.bytes_size;

	if (function->n_variables > 0)
		for (size_t i = 0; i < function->n_variables; ++i)
			*stack_bytes_size += function->variables[i].bytes_size;
}

// find position of variable on the stack (in bytes)
// returns -1 if not found, otherwise returns position on stack
static void _find_variable_position(
		const struct function_t* function,
		const char* variable_name,
		ssize_t* variable_stack_position,
		size_t* variable_bytes_size)
{
	for (size_t i = 0; i < function->n_variables; ++i)
	{
		*variable_stack_position += function->variables[i].bytes_size;
		if (strcmp(function->variables[i].name, variable_name) == 0)
		{
			*variable_bytes_size = function->variables[i].bytes_size;
			break;
		}
	}
}

static void _asm_function_write_instructions(
		FILE* asm_file,
		const struct function_t* function)
{
	for (size_t i = 0; i < function->n_instructions; ++i)
	{
		switch (function->instruction_code[i])
		{
			case INIT_VAR: // initialize variable
			{
				// TODO: this stack position does NOT currently
				// account for 7th+ parameters (which get allocated
				// on the stack)
				ssize_t variable_stack_position = 0;
				size_t variable_bytes_size = 0;

				_find_variable_position(
						function,
						function->instruction_arg1[i],
						&variable_stack_position,
						&variable_bytes_size);

				char mov_text[5];
				switch (variable_bytes_size)
				{
					case 1:
						memcpy(mov_text, "movb\0", 5);
						break;
					case 2:
						memcpy(mov_text, "movs\0", 5);
						break;
					case 4:
						memcpy(mov_text, "movl\0", 5);
						break;
					case 8:
						memcpy(mov_text, "movq\0", 5);
						break;
				}

				// TODO: check if value is a variable, then we'll
				// need to fetch its value from the stack and use
				// temp register to move it

				// for now (just to get things working) I will
				// assume everything is a constant

				fprintf(asm_file, "\t%s%s%s%s%s%zu%s\n", 
						mov_text, 
						" $", function->instruction_arg2[i], ",",
						"-", variable_stack_position, "(%rbp)");
			}

			default:
				break;
		}
	}
}

void asm_function_create(
		FILE* asm_file,
		const struct global_scope_t* global_scope)
{
	for (size_t i = 0; i < global_scope->n_functions; ++i)
	{
		// general boilerplate for creating a function
		fprintf(asm_file, "\n%s:\n", global_scope->functions[i].name);
		fprintf(asm_file, "\t%s\n", "pushq %rbp");
		fprintf(asm_file, "\t%s\n", "movq %rsp, %rbp");

		// allocate stack space for any local variables and/or 7th+ parameter (first six go into registers)
		if (global_scope->functions[i].n_parameters > 6 || global_scope->functions[i].n_variables > 0)
		{
			size_t stack_bytes_size = 0;
			_asm_function_allocate_stack(
					asm_file, 
					&global_scope->functions[i], 
					&stack_bytes_size);
			// allocate the actual stack space (e.g., subq $10, %rsp)
			fprintf(asm_file, "\t%s%zu%s\n", "subq $", stack_bytes_size, ", %rsp"); 
		}

		// translate function instructions into raw assembly
		_asm_function_write_instructions(asm_file, &global_scope->functions[i]);

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
}
