#include <assert.h>
#include <stdio.h>
#include "tokenizer.h"
#include "token_array.h"
#include "source_buffer.h"

int test_tokens(
	const struct token_array_t* array,
	const char (*expected_tokens)[10])
{
	if (array->length != 7)
	{
		fprintf(stderr, "Expected 7 tokens, got %zu\n", array->length);
		return -1;
	}

	for (size_t current_idx = 0; current_idx < array->length; ++current_idx)
	{
		const char* actual_token = array->token[current_idx].text;
		const char* expected_token = expected_tokens[current_idx];
		if (strcmp(actual_token, expected_token) != 0)
		{
			fprintf(stderr, "(Token# %zu/%zu): Expected token '%s' - got '%s'\n", 
				current_idx,
				array->length - 1,
				expected_token, 
				actual_token);
			return -1;
		}
	}

	return 0;

}

int main()
{
	char expected_tokens[7][10] = {
		"int32",
		"x",
		"=",
		"1",
		"+",
		"1",
		";"
	};

	struct source_buffer_t srcbuffer;
	srcbuffer_read("./assignment1.moca", &srcbuffer);

	struct token_array_t array;
	tknzer_extract_tokens(
		&array,
		&srcbuffer);

	int testval = test_tokens(&array, expected_tokens);
	if (testval == -1) return testval;

	srcbuffer_free(&srcbuffer);
	srcbuffer_read("./assignment2.moca", &srcbuffer);

	tknzer_extract_tokens(
		&array,
		&srcbuffer);

	return test_tokens(&array, expected_tokens);
}
