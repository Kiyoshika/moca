#include "parser_create_function_call.h"
#include "parser_create_function.h"
#include "parser_create_variable.h"
#include "parse_definition.h"
#include "token_array.h"
#include "err_msg.h"
#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "token.h"
#include "parameters.h"
#include "parser.h"

static bool _extract_function_name(
		char* function_call_name,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	memset(function_call_name, 0, FUNCTION_NAME_LEN);
	size_t function_call_name_len = 0;

	for (size_t i = 0; i < token_buffer->length; ++i)
	{
		char* new_name_token = token_buffer->token[i].text;

		if (function_call_name_len + strlen(new_name_token) > FUNCTION_NAME_LEN - 1)
		{
			err_write(
				err,
				"Function name cannot exceed 50 characters.",
				token_buffer->token[i].line_num,
				token_buffer->token[i].char_pos);

			return false;
		}

		strncat(function_call_name, new_name_token, strlen(new_name_token));
		function_call_name_len += strlen(new_name_token);
	}

	return false;
}

bool _find_function_index(
		struct global_scope_t* global_scope,
		char* function_name,
		ssize_t* function_idx,
		struct err_msg_t* err)
{
	for (size_t i = 0; i < global_scope->n_functions; ++i)
	{
		if (strncmp(global_scope->functions[i].name, function_name, FUNCTION_NAME_LEN) == 0)
		{
			*function_idx = i;
			return true;
		}
	}

	err_write(err, "Function name not found.", 0, 0);
	return false;
}

bool parser_create_function_call(
		struct global_scope_t* global_scope,
		struct function_t* function,
		struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		struct token_array_t* function_buffer,
		struct err_msg_t* err)
{
	// function buffer should contain every token leading up to open parenthesis.
	// we can use this to gather the function name
	char function_call_name[FUNCTION_NAME_LEN]; // defined in include/objects/functions.h
	bool success = _extract_function_name(function_call_name, function_buffer, err);
	if (!success)
		return false;

	// search the function in global scope; if not found, throws an error
	ssize_t function_idx = -1;
	success = _find_function_index(global_scope, function_call_name, &function_idx, err);
	if (!success)
	{
		err->line_num = token_buffer->token[*token_buffer_idx].line_num;
		err->char_pos = token_buffer->token[*token_buffer_idx].char_pos;
		return false;
	}

	return true;
}
