#include "parse_definition.h"
#include "parser_internal.c"
#include "parser.h"

bool parse_definition(
		struct token_array_t* token_array,
		size_t* token_array_idx,
		struct token_array_t* token_buffer,
		enum parse_definition_type* definition_type,
		bool allow_defining_functions,
		struct err_msg_t* err)
{
	bool contains_end_statement = false;

	// when first parsing a defintion, we expect a TEXT token immediately after
	// the data type
	enum token_type_e next_expected_token_types[5];
	enum token_category_e next_expected_token_categories[5];
	size_t next_expected_token_types_len = 1;
	size_t next_expected_token_categories_len = 1;
	next_expected_token_types[0] = TEXT;
	next_expected_token_categories[0] = NONE;

	// by default we'll assume we're creating a variable unless we hit
	// specific function token (open parenthesis)
	*definition_type = VARIABLE;

	while (true)
	{
		if (!token_is_expected(
					token_array->token[*token_array_idx].type,
					token_array->token[*token_array_idx].category,
					next_expected_token_types,
					next_expected_token_categories,
					next_expected_token_types_len,
					next_expected_token_categories_len))
		{
			err_write(err,
					"Unexpected token.",
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
				next_expected_token_types[0] = ASSIGNMENT;
				next_expected_token_types[1] = OPEN_PAREN;
				next_expected_token_types[2] = CLOSE_PAREN;
				next_expected_token_types[3] = END_STATEMENT;
				next_expected_token_types[4] = TEXT; // TODO: need to create actual COMMA token (this is just a cheap hack)
				next_expected_token_types_len = 5;

				next_expected_token_categories[0] = DATATYPE; // NOTE: this needs to be placed in "case COMMA:" after the COMMA token is created (see above TODO note)

				next_expected_token_categories_len = 1;

				break;
			}
			case OPEN_PAREN:
			{
				if (!allow_defining_functions)
				{
					err_write(err, "Cannot define a function here.", 
							token_array->token[*token_array_idx].line_num,
							token_array->token[*token_array_idx].char_pos);
					return false;
				}

				// text followed by open parenthesis indicates we
				// are creating a function
				*definition_type = FUNCTION;
				
				next_expected_token_types[0] = CLOSE_PAREN;
				next_expected_token_types_len = 1;

				next_expected_token_categories[0] = DATATYPE;
				next_expected_token_categories_len = 1;

				break;
			}
			case ASSIGNMENT:
			{
				next_expected_token_types[0] = TEXT;
				next_expected_token_types[1] = NUMBER;
				next_expected_token_types[2] = SUBTRACTION; // negative numbers
				next_expected_token_types_len = 4;

				next_expected_token_categories_len = 0;
				break;
			}
			case SUBTRACTION: // negative number
			{
				next_expected_token_types[0] = NUMBER;
				next_expected_token_types_len = 1;

				next_expected_token_categories_len = 0;
				break;
			}
			case NUMBER:
			{
				next_expected_token_types[0] = END_STATEMENT;
				next_expected_token_types_len = 1;

				next_expected_token_categories_len = 0;
				break;
			}
			case CLOSE_PAREN:
			{
				next_expected_token_types[0] = END_STATEMENT;
				next_expected_token_types[1] = OPEN_BRACE;
				next_expected_token_types_len = 2;

				next_expected_token_categories_len = 0;
				break;
			}
			// when reaching open brace, store ALL tokens between { and } into the token buffer,
			// these will be parsed in a moment
			case OPEN_BRACE:
			{
				// read tokens until closing brace
				while (*token_array_idx < token_array->length 
						&& token_array->token[*token_array_idx].type != CLOSE_BRACE)
				{
					tkn_array_push(token_buffer, &token_array->token[(*token_array_idx)++]);
				}

				// add closing brace (if present and not at end of file)
				if (*token_array_idx < token_array->length
						&& token_array->token[*token_array_idx].type == CLOSE_BRACE)
				{
					tkn_array_push(token_buffer, &token_array->token[(*token_array_idx)++]);
					contains_end_statement = true;
					goto endstatement;
				}
				break;
			}

			// stop parsing after ';' in variable definition
			case END_STATEMENT:
			{
				contains_end_statement = true;
				goto endstatement;
			}

			default:
				break;
		}

		switch (token_array->token[*token_array_idx].category)
		{
			case DATATYPE:
			{
				next_expected_token_types[0] = TEXT;
				next_expected_token_types_len = 1;

				next_expected_token_categories_len = 0;
				break;
			}

			default:
				break;
		}

		tkn_array_push(token_buffer, &token_array->token[(*token_array_idx)++]);

		if (*token_array_idx == token_array->length)
		{
			if (!contains_end_statement && *definition_type == VARIABLE)
			{
				err_write(err,
						"Missing semicolon.",
						token_array->token[*token_array_idx - 1].line_num,
						token_array->token[*token_array_idx - 1].char_pos);
				return false;
			}
			else if (!contains_end_statement && *definition_type == FUNCTION)
			{
				err_write(err,
						"Missing closing brace '}'.",
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
