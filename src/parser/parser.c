#include "parser.h"
#include "token_array.h"
#include "err_msg.h"

bool parse_tokens(
	struct token_array_t* token_array,
	struct err_msg_t* err)
{
	bool parse_success = true;

	// we parse tokens "on the fly" meaning we don't bother
	// constructing any tree first. we construct data structures
	// as we go and point to different locations in the
	// token_array as necessary
	

	// keep hold of a "token buffer" that holds contextual
	// information about what we're currently parsing. In
	// some cases we won't know what do to until we have
	// enough tokens (e.g., distinguishing between a variable
	// declaration and function declaration)
	struct token_array_t token_buffer;
	tkn_array_create(&token_buffer, 10);

	// NOTE: token_array_idx can be modified in the subparsers
	// and will be passed by address
	size_t token_array_idx = 0;

	// used when encountering a datatype (e.g., int32) which specifies a definition (variable/function)
	bool parsing_definition = false; 

	for (token_array_idx = 0; token_array_idx < token_array->length; ++token_array_idx)
	{
		switch (token_array->token[token_array_idx].category)
		{
			case DATATYPE:
			{
				// TODO: put this inside parse_definition.c

				// if we're already parsing a definition and encounter another datatype token, that's an error
				if (parsing_definition)
				{
					err_write(err,
							"Too many datatype tokens.",
							token_array->token[token_array_idx].line_num,
							token_array->token[token_array_idx].char_pos);
							
					parse_success = false;
					goto endparse;

				}
				parsing_definition = true;
				tkn_array_push(&token_buffer, &token_array->token[token_array_idx]);
			}

			default:
				break;				
		}
	}

endparse:
	// deallocate memory before leaving parser (e.g., when an error occurrs)
	tkn_array_free(&token_buffer);
	return parse_success;
}
