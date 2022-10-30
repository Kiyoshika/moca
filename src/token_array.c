#include "token_array.h"

bool tkn_array_create(
	struct token_array_t* token_array,
	const size_t capacity)
{
	struct token_t* tokens = calloc(capacity, sizeof(*tokens));
	if (!tokens)
		return false;

	token_array->tokens = tokens;
	token_array->capacity = capacity;
	token_array->length = 0;

	return true;
}

void tkn_array_free(
	struct token_array_t* token_array)
{
	free(token_array->tokens);
	token_array->tokens = NULL;
	
	token_array->length = 0;
	token_array->capacity = 0;
}

bool tkn_array_push(
	struct token_array_t* token_array,
	const struct token_t* token)
{
	memcpy(&token_array->tokens[token_array->length++], token, sizeof(*token));
	if (token_array->length == token_array->capacity)
	{
		void* alloc = realloc(token_array->tokens, sizeof(*token) * token_array->capacity * 2);
		if (!alloc)
			return false;
		token_array->tokens = alloc;
		token_array->capacity *= 2;
	}

	return true;
}
