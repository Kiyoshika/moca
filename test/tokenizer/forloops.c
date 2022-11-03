#include <assert.h>
#include <stdio.h>
#include "tokenizer.h"
#include "token_array.h"
#include "source_buffer.h"

#define N_TEST_TOKENS 14 // number of tokens per test line
#define N_TESTS 2 // number of test lines

#include "test_tokens.c"

int main()
{
	char expected_tokens[N_TEST_TOKENS * N_TESTS][10] = {
		"for", "(", "int32", "x", "=", "0", ";", "x", "<=", "5", ";", "++", "x", ")",
		"for", "(", "int32", "x", "=", "0", ";", "x", "<=", "5", ";", "++", "x", ")",
	};

	struct source_buffer_t srcbuffer;
	srcbuffer_read("./forloops.moca", &srcbuffer);

	struct token_array_t array;
	tknzer_extract_tokens(
		&array,
		&srcbuffer);

	int testval = test_tokens(&array, expected_tokens, 0, array.length);

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&array);

	return testval;

}
