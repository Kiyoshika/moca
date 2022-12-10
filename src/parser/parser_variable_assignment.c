#include "parser_variable_assignment.h"
#include "global_scope.h"
#include "variables.h"
#include "token_array.h"
#include "token.h"
#include "err_msg.h"

bool parser_variable_assignment(
		struct global_scope_t* global_scope,
		struct function_t* function,
		struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		struct token_array_t* function_buffer,
		struct err_msg_t* err)
{
	// TODO: can read tokens up until the space token and label
	// that as the variable name
	printf("\n\nVARIABLE ASSIGNMENT BUFFER (pre):\n");
	for (size_t i = 0; i < function_buffer->length; ++i)
		printf("TOKEN: %s\n", function_buffer->token[i].text);
	
	printf("\n\nVARIABLE ASSIGNMENT BUFFER (post):\n");
	while (*token_buffer_idx < token_buffer->length
			&& token_buffer->token[*token_buffer_idx].type != END_STATEMENT)
	{
		printf("TOKEN: %s\n", token_buffer->token[*token_buffer_idx].text);
		(*token_buffer_idx)++;
	}

	return true;
}
