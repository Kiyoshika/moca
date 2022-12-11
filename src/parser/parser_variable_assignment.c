#include "parser_variable_assignment.h"
#include "global_scope.h"
#include "variables.h"
#include "token_array.h"
#include "token.h"
#include "err_msg.h"
#include "util.h"
#include "functions.h"
#include "parameters.h"

bool parser_variable_assignment(
		struct global_scope_t* global_scope,
		struct function_t* function,
		struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		struct token_array_t* function_buffer,
		struct err_msg_t* err)
{
	// fetch variable name from buffer (may need to combine tokens from things
	// such as underscores)
	const enum token_type_e terminating_tokens[2] = { SPACE, END_STATEMENT };
	char var_name[VARIABLE_NAME_LEN];
	memset(var_name, 0, VARIABLE_NAME_LEN);

	size_t function_buffer_idx = 0;
	if (!util_get_name_from_buffer(
			var_name,
			function_buffer,
			&function_buffer_idx,
			terminating_tokens,
			2,
			VARIABLE_NAME_LEN,
			err))
		return false;

	// TODO: add support for global variables
	
	// check that the variable is defined in either the function parameters or stack
	printf("VARIABLE NAME: %s\n", var_name);
	size_t variable_idx = 0;
	ssize_t variable_stack_position = 0;
	size_t variable_bytes_size = 0;
	bool is_parameter = false;
	if (!util_find_variable_position(
				function,
				var_name,
				&variable_idx,
				&variable_stack_position,
				&variable_bytes_size,
				&is_parameter))
	{
		err_write(err, "Unknown variable name.", 0, 0);
		return false;
	}
	
	// pull all the contents on the right-hand side of assignment operator and
	// store them into a separate buffer
	struct token_array_t assignment_buffer;
	tkn_array_create(&assignment_buffer, 10);

	while (*token_buffer_idx < token_buffer->length
			&& token_buffer->token[*token_buffer_idx].type != END_STATEMENT)
	{
		if (token_buffer->token[*token_buffer_idx].type == SPACE)
		{
			(*token_buffer_idx)++;
			continue;
		}

		tkn_array_push(&assignment_buffer, &token_buffer->token[*token_buffer_idx]);
		(*token_buffer_idx)++;
	}

	// as of now, it only supports single assignment (no complicated expressions)
	// so using an entire new assignment buffer seems to be overkill for now
	if (assignment_buffer.length > 1)
	{
		err_write(err, "Variable assignment only supports single assignment (e.g., x = 5) - expressions are not yet supported.",
				assignment_buffer.token[0].line_num,
				assignment_buffer.token[1].char_pos);
		tkn_array_free(&assignment_buffer);
		return false;
	}

	// write instruction to assign a value:
	// arg1 = assignment variable name
	// arg2 = assignment value
	if (!function_write_instruction(
			function,
			ASSIGN_VAR,
			var_name,
			assignment_buffer.token[0].text,
			err))
	{
		tkn_array_free(&assignment_buffer);
		return false;
	}

	tkn_array_free(&assignment_buffer);

	return true;
}
