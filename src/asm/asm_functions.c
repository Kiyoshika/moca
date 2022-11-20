#include "asm_functions.h"
#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "parameters.h"
#include "err_msg.h"

void asm_create_text_section(FILE* asm_file)
{
	fprintf(asm_file, "%s\n", ".section .text");
	fprintf(asm_file, "%s\n", ".globl main");
}

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
		ssize_t* variable_stack_position,
		size_t* variable_bytes_size)
{
	for (size_t i = 0; i < function->n_variables; ++i)
	{
		*variable_stack_position += function->variables[i].bytes_size;
		if (strcmp(function->variables[i].name, variable_name) == 0)
		{
			*variable_bytes_size = function->variables[i].bytes_size;
			return true;
		}
	}

	return false;
}

static bool _asm_function_write_instructions(
		FILE* asm_file,
		const struct function_t* function,
		struct err_msg_t* err)
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
						memcpy(mov_text, "movw\0", 5);
						break;
					case 4:
						memcpy(mov_text, "movl\0", 5);
						break;
					case 8:
						memcpy(mov_text, "movq\0", 5);
						break;
				}

					// break this into its own function, I'm just lazy right now
					char variable_register_text[5];
					switch (variable_bytes_size)
					{
						case 1:
							memcpy(variable_register_text, "%bl\0", 4); // use lower half of B register for 8bit
							break;
						case 2:
							memcpy(variable_register_text, "%bx\0", 4);
							break;
						case 4:
							memcpy(variable_register_text, "%ebx\0", 5);
							break;
						case 8:
							memcpy(variable_register_text, "%rax\0", 5);
							break;
						default:
							break;
					}

				// this is the stack position of the variable's value
				// we're assigning (e.g., int32 x = [y] <--).
				// We will need to move this into a temp register (say, %rbx)
				// before moving that into the appropriate variable_stack_position
				ssize_t variable_assign_stack_position = 0;
				size_t variable_assign_bytes_size = 0;

				if (_check_is_variable(
						function->instruction_arg2[i]))
				{
					if (!_find_variable_position(
						function,
						function->instruction_arg2[i],
						&variable_assign_stack_position,
						&variable_assign_bytes_size))
					{
						// TODO: add line & char information in
						// variable_t, function_t, and parameter_t
						err_write(err, "Unknown variable name.", 0, 0);
						return false;
					}

					// TODO: break this into its own function (it's used twice)
					char assign_mov_text[5];
					switch (variable_assign_bytes_size)
					{
						case 1:
							memcpy(assign_mov_text, "movb\0", 5);
							break;
						case 2:
							memcpy(assign_mov_text, "movw\0", 5);
							break;
						case 4:
							memcpy(assign_mov_text, "movl\0", 5);
							break;
						case 8:
							memcpy(assign_mov_text, "movq\0", 5);
							break;
					}

					// TODO: break this into its own function
					char variable_assign_register_text[5];
					switch (variable_assign_bytes_size)
					{
						case 1:
							memcpy(variable_assign_register_text, "%bl\0", 4); // use lower half of B register for 8bit
							break;
						case 2:
							memcpy(variable_assign_register_text, "%bx\0", 4);
							break;
						case 4:
							memcpy(variable_assign_register_text, "%ebx\0", 5);
							break;
						case 8:
							memcpy(variable_assign_register_text, "%rax\0", 5);
							break;
					}

					// move assignment variable into temp register (i.e., int32 x = [y])
					// e.g., movq -12(%rbp), %rbx
					fprintf(asm_file, "\t%s%s%zu%s%s\n",
							assign_mov_text, " -", variable_assign_stack_position,
							"(%rbp), ", variable_assign_register_text);

					// move temp register into variable stack position
					// (the one we're assigning to, i.e., int32 [x] = y)
					// e.g., movq %rbx, -12(%rbp)
					// NOTE: we use the size of the left-hand variable, which may be larger
					// or smaller than the right-hand variable
					fprintf(asm_file, "\t%s%s%s%s%zu%s\n",
							mov_text, " ", variable_register_text, ", -", variable_stack_position, "(%rbp)");
				}
				else
				{
					// move constant into stack
					// e.g., movq $12, -15(%rbp)
					fprintf(asm_file, "\t%s%s%s%s%s%zu%s\n", 
							mov_text, 
							" $", function->instruction_arg2[i], ",",
							"-", variable_stack_position, "(%rbp)");
				}

			}

			default:
				break;
		}
	}

	return true;
}

bool asm_function_create(
		FILE* asm_file,
		const struct global_scope_t* global_scope,
		struct err_msg_t* err)
{
	for (size_t i = 0; i < global_scope->n_functions; ++i)
	{
		// general boilerplate for creating a function
		fprintf(asm_file, "\n%s:\n", global_scope->functions[i].name);
		fprintf(asm_file, "\t%s\n", "pushq %rbp");
		fprintf(asm_file, "\t%s\n", "movq %rsp, %rbp");

		// translate function instructions into raw assembly
		if (!_asm_function_write_instructions(asm_file, &global_scope->functions[i], err))
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