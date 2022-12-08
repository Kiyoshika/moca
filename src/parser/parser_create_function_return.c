#include "parser_create_function_return.h"
#include "functions.h"
#include "token_array.h"
#include "token.h"
#include "global_scope.h"
#include "err_msg.h"
#include "util.h"

bool parser_create_function_return(
		struct global_scope_t* global_scope,
		struct function_t* function,
		struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		struct token_array_t* function_buffer,
		struct err_msg_t* err)
{
	if (function->contains_return_statement)
	{
		err_write(err, "Function already contains a return statement.", 0, 0);
		return false;
	}

	// first token will be the RETURN token so we can skip over it
	(*token_buffer_idx)++;
	tkn_array_clear(function_buffer);

	while (*token_buffer_idx < token_buffer->length)
	{
		if (function_buffer->length == 1
				&& token_buffer->token[*token_buffer_idx].type != END_STATEMENT)
		{
			err_write(err, "As of now, return statement only supports a single variable or NUMERIC literal.",
					token_buffer->token[*token_buffer_idx].line_num,
					token_buffer->token[*token_buffer_idx].char_pos);
			return false;
		}
		else if (function_buffer->length == 1
				&& token_buffer->token[*token_buffer_idx].type == END_STATEMENT)
		{
			if (!function_write_instruction(
					function, 
					RETURN_FUNC, 
					function->name,
					function_buffer->token[0].text,
					err))
				return false;

			(*token_buffer_idx)++; // move past semicolon
			function->contains_return_statement = true;
			return true;
		}

		// skip spaces
		if (token_buffer->token[*token_buffer_idx].type == SPACE)
		{
			(*token_buffer_idx)++;
			continue;
		}

		// parse string literal (may need to be allocated)
		if (token_buffer->token[*token_buffer_idx].type == DOUBLE_QUOTE)
		{
			(*token_buffer_idx)++; // move past initial quote
			char* string_literal = util_get_text_between_quotes(
					token_buffer,
					token_buffer_idx,
					err);

			if (!string_literal)
				return false;

			if (!function_write_instruction(
					function, 
					RETURN_FUNC, 
					function->name,
					string_literal,
					err))
				return false;

			util_get_global_string_literal(
					global_scope,
					function,
					string_literal,
					err);

			free(string_literal);
			(*token_buffer_idx)++; // move past semicolon
			function->contains_return_statement = true;
			return true;
		}

		tkn_array_push(function_buffer, &token_buffer->token[*token_buffer_idx]);
		(*token_buffer_idx)++;
	}

	function->contains_return_statement = true;
	return true;
}
