#include <assert.h>
#include <stdio.h>
#include "tokenizer.h"
#include "token_array.h"
#include "source_buffer.h"

#define N_TEST_TOKENS 7 // number of tokens per test line
#define N_TESTS 4 // number of test lines

#include "test_tokens.c"

int main()
{
	char expected_tokens[N_TEST_TOKENS * N_TESTS][10] = {
		// we have four different variations of the below tokens
		"int32", "x", "=", "12", "+", "1", ";",
		"int32", "x", "=", "12", "+", "1", ";",
		"int32", "x", "=", "12", "+", "1", ";",
		"int32", "x", "=", "12", "+", "1", ";"
	};

	struct source_buffer_t srcbuffer;
	srcbuffer_read("./assignment.moca", &srcbuffer);

	struct token_array_t array;
	tknzer_extract_tokens(
		&array,
		&srcbuffer);

	int testval = test_tokens(&array, expected_tokens, 0, array.length);

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&array);

	return testval;

}
