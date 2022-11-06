#include "parse_definition.h"
#include "parser_internal.c"

bool parse_definition(
		struct token_array_t* token_array,
		size_t* token_array_idx,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	bool contains_end_statement = false;

	// when first parsing a defintion, we expect a TEXT token immediately after
	// the data type
	enum token_type_e next_expected_tokens[5];
	size_t expected_tokens_len = 1;
	next_expected_tokens[0] = TEXT;

	while (true)
	{
		if (!token_is_expected(
					token_array->token[*token_array_idx].type,
					next_expected_tokens,
					expected_tokens_len))
		{
			err_write(err,
					"Unexpected token type.",
					token_array->token[*token_array_idx].line_num,
					token_array->token[*token_array_idx].char_pos);
			return false;
		}

		// determine the next expected tokens (or end the statement)
		switch (token_array->token[*token_array_idx].type)
		{
			// NOTE: will need to rework this a little bit when building
			// strings that support quote assignments, e.g.,
			// string text = "hello world"
			// Would need a "inside_quote" to modify the expected tokens
			// to allow TEXT after TEXT if we're inside quotes

			case TEXT: // after text token, we expect one of "=", "(", ";"
			{
				next_expected_tokens[0] = ASSIGNMENT;
				next_expected_tokens[1] = OPEN_PAREN;
				next_expected_tokens[2] = END_STATEMENT;
				expected_tokens_len = 3;
				break;
			}
			case END_STATEMENT:
			{
				contains_end_statement = true;
				goto endstatement;
			}
			case OPEN_PAREN:
			{
				next_expected_tokens[0] = DATATYPE;
				next_expected_tokens[1] = CLOSE_PAREN;
				expected_tokens_len = 2;
				break;
			}
			case ASSIGNMENT:
			{
				next_expected_tokens[0] = TEXT;
				next_expected_tokens[1] = NUMBER;
				next_expected_tokens[2] = SUBTRACTION; // negative numbers
				expected_tokens_len = 4;
				break;
			}
			case SUBTRACTION: // negative number
			{
				next_expected_tokens[0] = NUMBER;
				expected_tokens_len = 1;
				break;
			}
			case NUMBER:
			{
				next_expected_tokens[0] = END_STATEMENT;
				expected_tokens_len = 1;
				break;
			}
			case CLOSE_PAREN:
			{
				next_expected_tokens[0] = END_STATEMENT;
				next_expected_tokens[1] = OPEN_BRACE;
				expected_tokens_len = 2;
				break;
			}
			default:
				break;
		}

		tkn_array_push(token_buffer, &token_array->token[(*token_array_idx)++]);

		if (*token_array_idx == token_array->length)
		{
			if (!contains_end_statement)
			{
				err_write(err,
						"Missing end statement (semicolon).",
						token_array->token[*token_array_idx - 1].line_num,
						token_array->token[*token_array_idx - 1].char_pos);
				return false;
			}
			return true;
		}

	}
endstatement:
	return true;
}
