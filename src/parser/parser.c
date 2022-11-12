#include "parser.h"
#include "token_array.h"
#include "err_msg.h"
#include "parse_definition.h"
#include "global_scope.h"
#include "parser_create_variable.h"
#include "parser_create_function.h"
#include "functions.h"

bool parse_tokens(
	struct token_array_t* token_array,
	struct global_scope_t* global_scope,
	struct err_msg_t* err)
{
	// the global scope context for the entire program
	if (!gscope_create(global_scope, err))
		goto endparse;

	// keep track of the current local scope we're in.
	// initially we'll be in global scope (NULL function scope)
	struct function_t* function_scope = NULL;

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

	for (token_array_idx = 0; token_array_idx < token_array->length; ++token_array_idx)
	{
		switch (token_array->token[token_array_idx].category)
		{
			// starting a statement with a data type indicates a definition
			// is being made
			case DATATYPE:
			{
				tkn_array_push(&token_buffer, &token_array->token[token_array_idx++]);

				enum parse_definition_type definition_type;
				parse_success = parse_definition(
						token_array,
						&token_array_idx,
						&token_buffer,
						&definition_type,
						true, // allow defining functions
						err);

				if (!parse_success)
					goto endparse;

				// after determining which variable type we parsed, we can
				// construct the object and add it to global/function scope
				switch (definition_type)
				{
					case VARIABLE:
						if (!function_scope)
							parser_create_global_variable(global_scope, &token_buffer, err);
						else
							parser_create_local_variable(function_scope, &token_buffer, err);
						break;

					case FUNCTION:
						parse_success = parser_create_function(global_scope, &token_buffer, err);
						if (!parse_success)
							goto endparse;
						token_array_idx--;
						break;

					default:
						break;
				}

				break;
			}

			default:
				break;				
		}

		tkn_array_clear(&token_buffer);
	}

endparse:
	// deallocate memory before leaving parser (e.g., when an error occurrs)
	tkn_array_free(&token_buffer);
	return parse_success;
}
