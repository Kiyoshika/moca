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
	bool inside_quotes = false;

	// when first parsing a defintion, we expect a TEXT token immediately after
	// the data type
	enum token_type_e next_expected_token_types[7];
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
				next_expected_token_types[4] = COMMA;
				next_expected_token_types[5] = SPACE;
				next_expected_token_types_len = 6;

				// allow any text inside quotes
				// by setting both lengths to 0
				if (inside_quotes)
					next_expected_token_types_len = 0;

				next_expected_token_categories_len = 0;

				break;
			}
			case COMMA: // data type MUST come after comma (e.g., writing function args)
			{
				next_expected_token_types_len = 0;

				next_expected_token_categories[0] = DATATYPE;
				next_expected_token_categories[1] = SPACE;
				next_expected_token_categories_len = 2;

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
				next_expected_token_types[1] = SPACE;
				next_expected_token_types_len = 2;

				next_expected_token_categories[0] = DATATYPE;
				next_expected_token_categories_len = 1;

				break;
			}
			case ASSIGNMENT:
			{
				next_expected_token_types[0] = TEXT;
				next_expected_token_types[1] = NUMBER;
				next_expected_token_types[2] = SUBTRACTION; // negative numbers
				next_expected_token_types[3] = DOUBLE_QUOTE; // for strings
				next_expected_token_types[4] = SPACE;
				next_expected_token_types_len = 5;

				next_expected_token_categories_len = 0;
				break;
			}
			case SUBTRACTION: // negative number
			{
				next_expected_token_types[0] = NUMBER;
				next_expected_token_types[1] = SPACE;
				next_expected_token_types_len = 2;

				next_expected_token_categories_len = 0;
				break;
			}
			case NUMBER:
			{
				next_expected_token_types[0] = END_STATEMENT;
				next_expected_token_types[1] = SPACE;
				next_expected_token_types_len = 2;

				next_expected_token_categories_len = 0;
				break;
			}
			case CLOSE_PAREN:
			{
				next_expected_token_types[0] = END_STATEMENT;
				next_expected_token_types[1] = OPEN_BRACE;
				next_expected_token_types[2] = SPACE;
				next_expected_token_types_len = 3;

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
			case DOUBLE_QUOTE:
			{
				inside_quotes = !inside_quotes;
				next_expected_token_types[0] = TEXT;
				next_expected_token_types[1] = DOUBLE_QUOTE;
				next_expected_token_types[2] = END_STATEMENT;
				next_expected_token_types[3] = SPACE;
				next_expected_token_types_len = 4;

				next_expected_token_categories_len = 0;
				break;
			}

			default:
				break;
		}

		switch (token_array->token[*token_array_idx].category)
		{
			case DATATYPE:
			{
				next_expected_token_types[0] = TEXT;
				next_expected_token_types[1] = SPACE;
				next_expected_token_types_len = 2;

				next_expected_token_categories_len = 0;
				break;
			}

			default:
				break;
		}

		// only add spaces to the buffer if we are insides quotes (for strings)
		// TODO: may need to do the same thing for TABS and NEWLINE characters
		if (!inside_quotes && token_array->token[*token_array_idx].type == SPACE)
		{
			(*token_array_idx)++;
			continue;
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
