#include "parser_create_variable.h"
#include "variables.h"
#include "err_msg.h"
#include "token_array.h"
#include "token.h"
#include "global_scope.h"
#include "functions.h"
#include "util.h"
#include <ctype.h>

static bool _set_and_return_error(
		struct err_msg_t* err,
		const struct token_array_t* token_buffer,
		const size_t token_idx)
{
	err->line_num = token_buffer->token[token_idx].line_num;
	err->char_pos = token_buffer->token[token_idx].char_pos;
	return false;
}

static bool _parse_variable(
		struct global_scope_t* global_scope,
		struct function_t* function,
		struct variable_t* new_variable,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	// TODO: there's got to be a much better way to organise this - it's pretty...messy

	size_t token_buffer_idx = 0;

	// first token is data type
	bool success = variable_set_type(new_variable, token_buffer->token[token_buffer_idx++].type, err);
	if (!success)
		return _set_and_return_error(err, token_buffer, token_buffer_idx - 1);	

	// second token is name
	// name can contain underscores and other stuff, 
	// so keep parsing until space occurs

	// first check if first character of name token is a letter.
	// don't names starting with random characters or numbers)
	if (!isalpha(token_buffer->token[token_buffer_idx].text[0]))
	{
		err_write(
			err, 
			"Variable name must start with a letter.",
			token_buffer->token[token_buffer_idx].line_num,
			token_buffer->token[token_buffer_idx].char_pos);

		return false;
	}

	char new_variable_name[VARIABLE_NAME_LEN]; // defined in include/objects/variables.h	
	memset(new_variable_name, 0, VARIABLE_NAME_LEN);
	size_t new_var_name_len = 0;
	
	// read variable name until '=' or ';' (space tokens are not included in the buffer)
	while (token_buffer_idx < token_buffer->length
			&& token_buffer->token[token_buffer_idx].type != ASSIGNMENT
			&& token_buffer->token[token_buffer_idx].type != END_STATEMENT)
	{
		char* new_name_token = token_buffer->token[token_buffer_idx].text;

		if (new_var_name_len + strlen(new_name_token) > VARIABLE_NAME_LEN - 1)
		{
			err_write(
				err,
				"Variable name cannot exceed 50 characters.",
				token_buffer->token[token_buffer_idx].line_num,
				token_buffer->token[token_buffer_idx].char_pos);

			return false;
		}

		strncat(new_variable_name, new_name_token, strlen(new_name_token));
		new_var_name_len += strlen(new_name_token);

		token_buffer_idx++;
	}

	success = variable_set_name(new_variable, new_variable_name, err);
	if (!success)
		return _set_and_return_error(err, token_buffer, token_buffer_idx);

	// if following token (after name) is not assignment, then
	// it's uninitialized
	if (token_buffer->token[token_buffer_idx++].type != ASSIGNMENT)
	{
		variable_set_initialized(new_variable, false);
		variable_set_value(new_variable, "0", err); // uninitialized are defaulted to 0
		return true;
	}

	// otherwise, we can set the value.
	// by the parser, third token is guaranteed to be assignment, so we
	// can directly read fourth/fifth token as its value
	variable_set_initialized(new_variable, true);

	// if next token is double quote then it's a string
	if (token_buffer->token[token_buffer_idx].type == DOUBLE_QUOTE)
	{
		// validate correct data type
		if (token_buffer->token[0].type != STRING)
		{
			err_write(err, "Data type is not a 'string'.", 0, 0);
			return _set_and_return_error(err, token_buffer, token_buffer_idx);
		}

		size_t initial_position = token_buffer_idx;
		token_buffer_idx++; // move past the double quote
		char* str_value = util_get_text_between_quotes(token_buffer, &token_buffer_idx, err);

		if (!str_value)
			return _set_and_return_error(err, token_buffer, initial_position);

		util_get_global_string_literal(
				global_scope,
				function,
				str_value,
				err);

		return true;
	}

	// if fourth token is "-" then it's a negative number. 
	if (token_buffer->token[token_buffer_idx].text[0] == '-')
	{
		char* val = calloc(strlen(token_buffer->token[token_buffer_idx + 1].text) + 2, sizeof(char));
		val[0] = '-';
		strncat(val, token_buffer->token[token_buffer_idx + 1].text, strlen(token_buffer->token[token_buffer_idx + 1].text));
		success = variable_set_value(new_variable, val, err);
	}
	else
		success = variable_set_value(new_variable, token_buffer->token[token_buffer_idx].text, err);

	if (!success)
		return _set_and_return_error(err, token_buffer, token_buffer_idx);

	return true;
}

bool parser_create_global_variable(
		struct global_scope_t* global_scope,
		struct function_t* function,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	struct variable_t new_variable;
	variable_create(&new_variable);

	bool success = _parse_variable(global_scope, function, &new_variable, token_buffer, err);
	if (!success)
		return false;

	/*success = gscope_add_variable(global_scope, &new_variable, err);
	if (!success)
		return false;*/

	tkn_array_clear(token_buffer);

	return true;

}

bool parser_create_local_variable(
		struct global_scope_t* global_scope,
		struct function_t* function_scope,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	struct variable_t new_variable;
	variable_create(&new_variable);

	size_t global_vars = global_scope->n_variables;
	bool success = _parse_variable(global_scope, function_scope, &new_variable, token_buffer, err);
	if (!success)
		return false;

	// only add to function if variable was not globally allocated (e.g., string)
	if (global_vars == global_scope->n_variables)
		success = function_add_variable(function_scope, &new_variable, err);

	if (!success)
		return false;

	tkn_array_clear(token_buffer);

	return true;
}
