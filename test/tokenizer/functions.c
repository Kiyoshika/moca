#include <assert.h>
#include <stdio.h>
#include "tokenizer.h"
#include "token_array.h"
#include "source_buffer.h"

#define N_TEST_TOKENS 6 // number of tokens per test line
#define N_TESTS 1 // number of test lines

#include "test_tokens.c"

int main()
{
	// each test line is different length, so we separate them into different arrays

	char expected_tokens1[N_TEST_TOKENS * N_TESTS][10] = {
		"void", "function", "(", ")", "{", "}"
	};

	#undef N_TEST_TOKENS
	#define N_TEST_TOKENS 11
	char expected_tokens2[N_TEST_TOKENS * N_TESTS][10] = {
		"void", "function", "(", "int32", "x", ",", "int32", "y", ")", "{", "}"
	};

	#undef N_TEST_TOKENS
	#define N_TEST_TOKENS 13

	#undef N_TESTS
	#define N_TESTS 2
	char expected_tokens3[N_TEST_TOKENS * N_TESTS][10] = {
		"void", "function", "(", "int32", "&", "x", ",", "int32", "&", "y", ")", "{", "}",
		"void", "function", "(", "int32", "&", "x", ",", "int32", "&", "y", ")", "{", "}"

	};

	struct source_buffer_t srcbuffer;
	srcbuffer_read("./functions.moca", &srcbuffer);

	struct token_array_t array;
	tknzer_extract_tokens(
		&array,
		&srcbuffer);

	int result = 0;

	int testval = test_tokens(&array, expected_tokens1, 0, 5);
	if (testval == -1)
		result = -1;

	testval = test_tokens(&array, expected_tokens2, 6, 16);
	if (testval == -1)
		result = -1;

	testval = test_tokens(&array, expected_tokens3, 17, 42);
	if (testval == -1)
		result = -1;

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&array);

	return result;

}
