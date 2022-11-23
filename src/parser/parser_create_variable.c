#include "parser_create_variable.h"
#include "variables.h"
#include "err_msg.h"
#include "token_array.h"
#include "token.h"
#include "global_scope.h"
#include "functions.h"
#include <ctype.h>

static bool _text_realloc(
		char** text,
		size_t* text_capacity)
{
	void* alloc = realloc(*text, *text_capacity * 2);
	if (!alloc)
		return false;

	*text = alloc;
	*text_capacity *= 2;

	return true;
}

static bool _set_and_return_error(
		struct err_msg_t* err,
		const struct token_array_t* token_buffer,
		const size_t token_idx)
{
	err->line_num = token_buffer->token[token_idx].line_num;
	err->char_pos = token_buffer->token[token_idx].char_pos;
	return false;
}

static char* _get_text_between_quotes(
		struct token_array_t* token_buffer,
		size_t* buffer_idx,
		struct err_msg_t* err)
{
	// start off with 100 bytes, but may need to reallocate more depending on how large the string is
	size_t text_len = 0;
	size_t text_capacity = 100;
	char* text = malloc(text_capacity);

	if (!text)
	{
		err_write(err, "Couldn't allocate memory for string.", 0, 0);
		return NULL;
	}

	memset(text, 0, text_capacity);
	text[0] = '"'; // start text value with single quote
	text_len++;

	// iterate everything in between double quotes
	// NOTE: it's also possible that there is no ending quote which is an error,
	// we will check for this
	bool contains_end_quote = false;
	while (*buffer_idx < token_buffer->length)
	{
		if (token_buffer->token[*buffer_idx].type == DOUBLE_QUOTE)
		{
			contains_end_quote = true;
			if (text_len + 2 >= text_capacity)
			{
				if (!_text_realloc(&text, &text_capacity))
				{
					err_write(err, "Couldn't allocate memory for string.", 0, 0);
					free(text);
					return NULL;
				}
			}
			
			text[text_len++] = '"';
			text_len++;
			break;
		}

		char* new_text = token_buffer->token[*buffer_idx].text;
		size_t new_text_len = strlen(new_text);

		if (text_len + new_text_len >= text_capacity)
		{
			if (!_text_realloc(&text, &text_capacity))
			{
				err_write(err, "Couldn't allocate memory for string.", 0, 0);
				free(text);
				return NULL;
			}
		}

		strncat(text, new_text, new_text_len);
		text_len += new_text_len;

		(*buffer_idx)++;
	}

	if (!contains_end_quote)
	{
		err_write(err, "Missing end quote for string.", 0, 0);
		free(text);
		return NULL;
	}
	
	return text;
}

static bool _parse_variable(
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
		char* str_value = _get_text_between_quotes(token_buffer, &token_buffer_idx, err);

		if (!str_value)
			return _set_and_return_error(err, token_buffer, initial_position);

		variable_set_value(new_variable, str_value, err);
		free(str_value);

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
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	printf("\n\nGLOBAL VARIABLE BUFFER:\n");
	for (size_t i = 0; i < token_buffer->length; ++i)
		printf("%s\n", token_buffer->token[i].text);

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
	printf("\n\nLOCAL VARIABLE BUFFER:\n");
	for (size_t i = 0; i < token_buffer->length; ++i)
		printf("%s\n", token_buffer->token[i].text);

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
