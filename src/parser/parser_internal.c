bool token_is_expected(
		enum token_type_e current_token_type,
		enum token_category_e current_token_category,
		enum token_type_e* expected_token_types,
		enum token_category_e* expected_token_categories,
		size_t expected_token_types_len,
		size_t expected_token_categories_len)
{
	// if both lengths are set to 0, allow any token
	// (currently only used for parsing strings to allow arbitrary
	// text inside)
	if (expected_token_types_len == 0
		&& expected_token_categories_len == 0)
		return true;

	// need to support searching either token category or token types.
	// the lazy way is to just iterate over both loops...not really "efficient"
	// but these loops are very small so it doesn't really matter
	
	for (size_t i = 0; i < expected_token_types_len; ++i)
		if (current_token_type == expected_token_types[i]) return true;

	for (size_t i = 0; i < expected_token_categories_len; ++i)
		if (current_token_category == expected_token_categories[i]) return true;

	return false;
}
