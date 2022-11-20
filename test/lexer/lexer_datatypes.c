#include "tokenizer.h"
#include "token_array.h"
#include "source_buffer.h"
#include "lexer.h"
#include <stdio.h>
#include <stddef.h>

#define N_TEST_TOKENS 5
int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./datatypes.moca", &srcbuffer);
	
	struct token_array_t array;
	tknzer_extract_tokens(
		&array,
		&srcbuffer);

	int testval = 0;

	enum token_type_e expected_types[N_TEST_TOKENS] = {
		INT8,
		INT16,
		INT32,
		INT64,
		STRING
	};

	lexer_parse(&array);

	for (size_t i = 0; i < array.length; ++i)
	{
		if (array.token[i].type != expected_types[i])
		{
			testval = -1;
			char* expected_token = type_to_text(expected_types[i]);
			fprintf(stderr, "Token %zu/%zu: Expected type '%s' - got '%s'\n", i+1, (size_t)N_TEST_TOKENS, expected_token, type_to_text(array.token[i].type));
			break;
		}

		if (array.token[i].category != DATATYPE)
		{
			testval = -1;
			fprintf(stderr, "Token %zu/%zu: Expected category 'NONE' - got '%s'\n", i+1, (size_t)N_TEST_TOKENS, category_to_text(array.token[i].category));
			break;
		}
	}

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&array);

	return testval;
}
