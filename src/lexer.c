#include "lexer.h"
#include "token_array.h"

static int cmp(const void* a, const void* b)
{
	const char* _a = (const char*)a;
	const char* _b = (const char*)b;

	return strcmp(_a, _b);
}

void lexer_parse(
	struct token_array_t* token_array)
{
	for (size_t i = 0; i < token_array->length; ++i)
	{
		void* found_token = bsearch(
			token_array->token[i].text,
			token_list, 		// defined in token.h
			N_TOKENS, 			// defined in token.h
			MAX_TOKEN_LIST_LEN, // defined in token.h
			&cmp);

		void* token_list_base = token_list;
		
		if (!found_token)
		{
			char* endptr = NULL;
			strtod(token_array->token[i].text, &endptr);

			if (strlen(endptr) == 0)
				token_array->token[i].type = NUMBER;
			else
				token_array->token[i].type = TEXT;

			token_array->token[i].category = NONE;
		}
		else
		{
			size_t idx = (found_token - token_list_base)/MAX_TOKEN_LIST_LEN;
			token_array->token[i].type = idx;
			switch (idx)
			{
				case INT8:
				case INT16:
				case INT32:
				case INT64:
				case STRING:
					token_array->token[i].category = DATATYPE;
					break;

				case ADDITION:
				case SUBTRACTION:
				case DIVISION:
				case MULTIPLICATION:
				case MODULO:
				case ASSIGNMENT:
				case INCREMENT:
				case DECREMENT:
				case PLUS_EQUALS:
				case MINUS_EQUALS:
					token_array->token[i].category = OPERATOR;
					break;

				case GREATER_THAN:
				case GREATER_THAN_EQ:
				case LESS_THAN:
				case LESS_THAN_EQ:
				case EQUAL:
				case NOT_EQUAL:
				case LOGICAL_AND:
				case LOGICAL_OR:
					token_array->token[i].category = LOGICAL;
					break;

				default:
					token_array->token[i].category = NONE;
					break;
				
			}
		}
	}
}
