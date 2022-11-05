/*
 * INTERNAL FUNCTIONS FOR tokenizer.c
 *
 * THIS WILL BE COPIED (via include) DIRECTLY INTO tokenizer.c
 */

// split tokens on non-alpha/numeric characters
static inline bool is_tokenize_char(const char tokenize_char)
{
	return isalnum(tokenize_char) == 0;
}

// verify that a full stop '.' is not a decimal (e.g., .33 or 5.93)
static bool is_not_decimal(
	const char char_value,
	const char next_char_value)
{
	if (char_value == '.' && next_char_value >= '0' && next_char_value <= '9')
		return false;
	
	return true;

}

static bool ignore_char(const char char_value)
{
	switch (char_value)
	{
		case '\n':
		case ' ':
		case '\t':
			return true;
		default:
			return false;
	}

	return false;
}

static void push_into_token_array(
	struct token_t* current_token,
	struct token_array_t* token_array,
	char* current_buff,
	size_t current_line_num,
	size_t current_char,
	size_t* current_buff_idx)
{
	tkn_create(current_token, current_buff, current_line_num + 1, current_char - strlen(current_buff) + 1);
	tkn_array_push(token_array, current_token);
	memset(current_buff, 0, MAX_LINE_LEN);
	*current_buff_idx = 0;

}

// special double tokens such as '!=', '&&', '++', etc.
static bool is_double_token(const char current_char, const char next_char)
{
	switch (current_char)
	{
		case '+':
		{
			switch (next_char)
			{
				case '+': // ++
					return true;
				case '=': // +=
					return true;
				default:
					return false;
			}
		}

		case '-':
		{
			switch (next_char)
			{
				case '-': // --
					return true;
				case '=': // -=
					return true;
				default:
					return false;
			}
		}

		case '=':
		{
			switch (next_char)
			{
				case '=': // ==
					return true;
				default:
					return false;
			}
		}

		case '|':
		{
			switch (next_char)
			{
				case '|': // ||
					return true;
				default:
					return false;
			}
		}

		case '<':
		{
			switch (next_char)
			{
				case '=': // <=
					return true;
				default:
					return false;
			}
		}

		case '>':
		{
			switch (next_char)
			{
				case '=': // >=
					return true;
				default:
					return false;
			}
		}

		case '!':
		{
			switch (next_char)
			{
				case '=': // !=
					return true;
				default:
					return false;
			}
		}

		case '&':
		{
			switch (next_char)
			{
				case '&': // &&
					return true;
				default:
					return false;
			}
		}
	}

	return false;
}
