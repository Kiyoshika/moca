#include "token_array.h"

bool tkn_array_create(
	struct token_array_t* token_array,
	const size_t capacity)
{
	struct token_t* tokens = calloc(capacity, sizeof(*tokens));
	if (!tokens)
		return false;

	token_array->token = tokens;
	token_array->capacity = capacity;
	token_array->length = 0;

	return true;
}

void tkn_array_free(
	struct token_array_t* token_array)
{
	free(token_array->token);
	token_array->token = NULL;
	
	token_array->length = 0;
	token_array->capacity = 0;
}

bool tkn_array_push(
	struct token_array_t* token_array,
	const struct token_t* token)
{
	memcpy(&token_array->token[token_array->length++], token, sizeof(*token));
	if (token_array->length == token_array->capacity)
	{
		void* alloc = realloc(token_array->token, sizeof(*token) * token_array->capacity * 2);
		if (!alloc)
			return false;
		token_array->token = alloc;
		token_array->capacity *= 2;
	}

	return true;
}

void tkn_array_clear(
	struct token_array_t* token_array)
{
	// technically only need to use token_array->length but doing capacity
	// just to be safe...
	memset(token_array->token, 0, token_array->capacity * sizeof(*token_array->token));
	token_array->length = 0;
}
