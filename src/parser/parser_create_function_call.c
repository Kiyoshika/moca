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
#include "built_in_functions.h"
#include "util.h"

static bool _extract_function_name(
		char* function_call_name,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	memset(function_call_name, 0, FUNCTION_NAME_LEN);
	size_t function_call_name_len = 0;

	for (size_t i = 0; i < token_buffer->length; ++i)
	{
		if (token_buffer->token[i].type == OPEN_PAREN)
			break;

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

	return true;
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

static char* _concat_arg_buffer(
	const struct token_array_t* arg_buffer,
	struct err_msg_t* err)
{
	size_t arg_str_capacity = 10;
	char* arg_str = malloc(arg_str_capacity);
	size_t arg_str_len = 0;
	if (!arg_str)
	{
		err_write(err, "Couldn't allocate memory for parameters when making function call.", 0, 0);
		return NULL;
	}
	memset(arg_str, 0, arg_str_capacity);

	for (size_t i = 0; i < arg_buffer->length; ++i)
	{
		char* next_token = arg_buffer->token[i].text;
		if (arg_str_len + strlen(next_token) > arg_str_capacity)
		{
			void* alloc = realloc(arg_str, arg_str_capacity * 2);
			if (!alloc)
			{
				free(arg_str);
				err_write(err, "Couldn't allocate memory for parameters when making function call.", 0, 0);
				return NULL;
			}
			arg_str_capacity *= 2;
			arg_str = alloc;
		}
		strncat(arg_str, next_token, strlen(next_token));
		arg_str_len += strlen(next_token);
	}

	return arg_str;
	
}

static bool _add_arg_to_function(
	struct function_t* function,
	char* function_name,
	struct token_array_t* arg_buffer,
	struct err_msg_t* err)
{
	char* arg_str = _concat_arg_buffer(arg_buffer, err);
	if (!arg_str)
	{
		tkn_array_free(arg_buffer);
		return false;
	}
	bool success = function_write_instruction(
		function, 
		ADD_ARG, 
		function_name,
		arg_str,
		err);
	free(arg_str);
	if (!success)
	{
		tkn_array_free(arg_buffer);
		return false;
	}
	tkn_array_clear(arg_buffer);
	return true;
}

static bool _parse_function_call_args(
		struct function_t* function,
		char* function_name,
		struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		struct err_msg_t* err)
{
	bool function_is_closed = false;
	size_t open_paren_index = *token_buffer_idx;

	struct token_array_t arg_buffer;
	tkn_array_create(&arg_buffer, 10);

	// keep reading tokens until ')' is found (or not...if function is left open)
	while (*token_buffer_idx < token_buffer->length)
	{
		switch (token_buffer->token[*token_buffer_idx].type)
		{
			case CLOSE_PAREN:
				function_is_closed = true;
				goto endwhile;
				break;

			case DOUBLE_QUOTE:
				(*token_buffer_idx)++; // skip first double quote
				char* str_value = util_get_text_between_quotes(token_buffer, token_buffer_idx, err);
				if (!str_value)
				{
					err_write(
						err, 
						"Problem with parsing string inside function argument.",
						token_buffer->token[*token_buffer_idx].line_num, 
						token_buffer->token[*token_buffer_idx].char_pos);
					return false;
				}
				struct token_t temp_token;
				tkn_create(
					&temp_token, 
					str_value,
					token_buffer->token[*token_buffer_idx].line_num,
					token_buffer->token[*token_buffer_idx].char_pos);
				temp_token.type = STRING;
				temp_token.category = DATATYPE;

				free(str_value);

				tkn_array_push(&arg_buffer, &temp_token);
				(*token_buffer_idx)++; // skip last double quote after parsing string
				break;

			case COMMA: 
				(*token_buffer_idx)++;
				_add_arg_to_function(function, function_name, &arg_buffer, err);
				tkn_array_clear(&arg_buffer);
				break;

			default:
				if (token_buffer->token[*token_buffer_idx].type != SPACE) // don't add spaces to arg buffer
					tkn_array_push(&arg_buffer, &token_buffer->token[*token_buffer_idx]);
				(*token_buffer_idx)++;
				break;
		}
	}

endwhile:
	if (arg_buffer.length > 0)
	{
		bool success = _add_arg_to_function(function, function_name, &arg_buffer, err);
		if (!success)
			return false;
	}

	if (!function_is_closed)
	{
		err_write(
			err, 
			"Missing closing parenthesis on function.",
			token_buffer->token[open_paren_index].line_num,
			token_buffer->token[open_paren_index].char_pos);
		tkn_array_free(&arg_buffer);
		return false;
	}

	tkn_array_free(&arg_buffer);
	(*token_buffer_idx)++;
	return true;
}

static bool _is_reserved_function_name(const char* function_call_name)
{
	for (size_t i = 0; i < (size_t)N_BUILT_IN_FUNCTIONS; ++i)
		if (strcmp(function_call_name, built_in_functions[i]) == 0)
			return true;

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

	if (!_is_reserved_function_name(function_call_name))
	{
		// search the function in global scope; if not found, throws an error
		ssize_t function_idx = -1;
		success = _find_function_index(global_scope, function_call_name, &function_idx, err);
		if (!success)
		{
			err->line_num = token_buffer->token[*token_buffer_idx].line_num;
			err->char_pos = token_buffer->token[*token_buffer_idx].char_pos;
			return false;
		}
	}

	// extract arguments and write a ADD_ARG instruction which takes
	// the function name as arg1 and the variable/value as arg2
	_parse_function_call_args(function, function_call_name, token_buffer, token_buffer_idx, err);

	// then finally create the CALL_FUN instruction which takes
	// the name of the function as arg1 and NULL as arg2
	function_write_instruction(function, CALL_FUNC, function_call_name, NULL, err);

	return true;
}
