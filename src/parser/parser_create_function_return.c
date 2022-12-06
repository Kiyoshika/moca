#include "parser_create_function_return.h"
#include "functions.h"
#include "token_array.h"
#include "token.h"
#include "global_scope.h"
#include "err_msg.h"

bool parser_create_function_return(
		struct global_scope_t* global_scope,
		struct function_t* function,
		struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		struct token_array_t* function_buffer,
		struct err_msg_t* err)
{
	// first token will be the RETURN token so we can skip over it
	(*token_buffer_idx)++;
	tkn_array_clear(function_buffer);

	while (*token_buffer_idx < token_buffer->length)
	{
		if (function_buffer->length == 1
				&& token_buffer->token[*token_buffer_idx].type != END_STATEMENT)
		{
			err_write(err, "As of now, return statement only supports a single variable or literal.",
					token_buffer->token[*token_buffer_idx].line_num,
					token_buffer->token[*token_buffer_idx].char_pos);
			return false;
		}
		else if (function_buffer->length == 1
				&& token_buffer->token[*token_buffer_idx].type == END_STATEMENT)
		{
			// TODO: write function instruction to return
			(*token_buffer_idx)++; // move past semicolon
			return true;
		}

		// skip spaces
		if (token_buffer->token[*token_buffer_idx].type == SPACE)
		{
			(*token_buffer_idx)++;
			continue;
		}

		tkn_array_push(function_buffer, &token_buffer->token[*token_buffer_idx]);

		//printf("%s\n", token_buffer->token[*token_buffer_idx].text);
		(*token_buffer_idx)++;
	}

	return true;
}
