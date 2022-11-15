#include "parser_create_variable.h"
#include "variables.h"
#include "err_msg.h"
#include "token_array.h"
#include "token.h"
#include "global_scope.h"
#include "functions.h"

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
		struct variable_t* new_variable,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	// first token is data type
	bool success = variable_set_type(new_variable, token_buffer->token[0].type, err);
	if (!success)
		return _set_and_return_error(err, token_buffer, 0);	

	// second token is name
	success = variable_set_name(new_variable, token_buffer->token[1].text, err);
	if (!success)
		return _set_and_return_error(err, token_buffer, 1);

	// if no third token, this variable is uninitialized
	if (token_buffer->length == 2)
	{
		variable_set_initialized(new_variable, false);
		variable_set_value(new_variable, "0", err); // uninitialized are defaulted to 0
		return true;
	}

	// otherwise, we can set the value.
	// by the parser, third token is guaranteed to be assignment, so we
	// can directly read fourth/fifth token as its value
	variable_set_initialized(new_variable, true);

	// if fourth token is "-" then it's a negative number. 
	if (token_buffer->token[3].text[0] == '-')
	{
		char* val = calloc(strlen(token_buffer->token[4].text) + 2, sizeof(char));
		val[0] = '-';
		strncat(val, token_buffer->token[4].text, strlen(token_buffer->token[4].text));
		success = variable_set_value(new_variable, val, err);
	}
	else
		success = variable_set_value(new_variable, token_buffer->token[3].text, err);

	if (!success)
		return _set_and_return_error(err, token_buffer, 3);

	return true;
}

bool parser_create_global_variable(
		struct global_scope_t* global_scope,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{

	struct variable_t new_variable;
	variable_create(&new_variable);

	bool success = _parse_variable(&new_variable, token_buffer, err);
	if (!success)
		return false;

	success = gscope_add_variable(global_scope, &new_variable, err);
	if (!success)
		return false;

	tkn_array_clear(token_buffer);

	return true;

}

bool parser_create_local_variable(
		struct function_t* function_scope,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	struct variable_t new_variable;
	variable_create(&new_variable);

	bool success = _parse_variable(&new_variable, token_buffer, err);
	if (!success)
		return false;

	success = function_add_variable(function_scope, &new_variable, err);
	if (!success)
		return false;

	tkn_array_clear(token_buffer);

	return true;
}
