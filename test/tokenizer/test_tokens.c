int test_tokens(
	const struct token_array_t* array,
	const char (*expected_tokens)[10],
	const size_t array_start_idx,
	const size_t array_end_idx)
{
	/*if (array->length != N_TEST_TOKENS * N_TESTS)
	{
		fprintf(stderr, "Expected %zu total tokens, got %zu\n", 
			(size_t)(N_TEST_TOKENS * N_TESTS),
			array->length);

		return -1;
	}*/

	for (size_t current_idx = array_start_idx; current_idx < array_end_idx; ++current_idx)
	{
		const char* actual_token = array->token[current_idx].text;
		const char* expected_token = expected_tokens[current_idx - array_start_idx];
		if (strcmp(actual_token, expected_token) != 0)
		{
			fprintf(stderr, "Line: %zu, Token: %zu/%zu, Expected token '%s' - got '%s'\n", 
				current_idx/N_TEST_TOKENS+ 1,
				current_idx % N_TEST_TOKENS+ 1,
				(size_t)N_TEST_TOKENS,
				expected_token, 
				actual_token);
			return -1;
		}
	}

	return 0;

}
