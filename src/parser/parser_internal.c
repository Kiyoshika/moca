bool token_is_expected(
		enum token_type_e current_token,
		enum token_type_e* expected_tokens,
		size_t expected_tokens_len)
{
	for (size_t i = 0; i < expected_tokens_len; ++i)
		if (current_token == expected_tokens[i]) return true;

	return false;
}
