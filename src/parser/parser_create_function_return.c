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
		struct err_msg_t* err_msg)
{
	// first token will be the RETURN token so we can skip over it
	(*token_buffer_idx)++;

	printf("\n\nRETURN BUFFER:\n");
	while (*token_buffer_idx < token_buffer->length)
	{
		// skip spaces
		if (token_buffer->token[*token_buffer_idx].type == SPACE)
		{
			(*token_buffer_idx)++;
			continue;
		}

		printf("%s\n", token_buffer->token[*token_buffer_idx].text);
		(*token_buffer_idx)++;
	}

	return true;
}
