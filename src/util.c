#include "util.h"
#include "token_array.h"
#include "token.h"
#include "err_msg.h"
#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "err_msg.h"

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

static bool _is_terminating_token(
		const enum token_type_e token,
		const enum token_type_e* terminating_tokens,
		const size_t n_terminating_tokens)
{
	for (size_t i = 0; i < n_terminating_tokens; ++i)
		if (token == terminating_tokens[i])
			return true;

	return false;
}

char* util_get_text_between_quotes(
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
			//text_len++;
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

size_t util_get_global_string_literal(
		struct global_scope_t* global_scope,
		const struct function_t* function,
		const char* string_literal_value,
		struct err_msg_t* err)
{
	bool needs_allocation = true;
	size_t global_var_idx = 0;
	for (size_t i = 0; i < global_scope->n_variables; ++i)
	{
		if (strcmp(string_literal_value, global_scope->variables[i].value) == 0)
		{
			needs_allocation = false;
			global_var_idx = i;
		}
	}

	if (needs_allocation)
	{
		// add to global scope with a name created from
		// the current function name and global scope idx
		char name[VARIABLE_NAME_LEN];
		memset(name, 0, VARIABLE_NAME_LEN);
		if (function)
			memcpy(name, function->name, 47); // use up to 47 chars of function name
		char inttostr[100]; // maximum of 999 global variables supported
		sprintf(inttostr, "%zu", global_scope->n_variables);
		strncat(name, inttostr, 3);

		struct variable_t new_global;
		variable_create(&new_global);
		variable_set_type(&new_global, STRING, err);
		variable_set_name(&new_global, name, err);
		variable_set_value(&new_global, string_literal_value, err);
		variable_set_initialized(&new_global, true);

		if (!gscope_add_variable(global_scope, &new_global, err))
			return false;

		global_var_idx = global_scope->n_variables - 1;
	}

	return global_var_idx;
}

bool util_get_name_from_buffer(
		char* name_buffer,
		const struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		const enum token_type_e* terminating_tokens,
		const size_t n_terminating_tokens,
		const size_t max_name_len,
		struct err_msg_t* err)
{
	size_t new_name_len = 0;
	while (*token_buffer_idx < token_buffer->length
			&& !_is_terminating_token(
				token_buffer->token[*token_buffer_idx].type,
				terminating_tokens,
				n_terminating_tokens))
	{
		char* new_name_token = token_buffer->token[*token_buffer_idx].text;

		if (new_name_len + strlen(new_name_token) > max_name_len - 1)
		{
			err_write(
				err,
				"Token name too long.",
				token_buffer->token[*token_buffer_idx].line_num,
				token_buffer->token[*token_buffer_idx].char_pos);

			return false;
		}

		strncat(name_buffer, new_name_token, strlen(new_name_token));
		new_name_len += strlen(new_name_token);

		(*token_buffer_idx)++;
	}

	return true;
}
