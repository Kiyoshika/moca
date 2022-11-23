#include "parser_create_function_call.h"
#include "parser_create_function.h"
#include "parser_create_variable.h"
#include "parse_definition.h"
#include "token_array.h"
#include "err_msg.h"
#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "token.h"
#include "parameters.h"
#include "parser.h"

// read token buffer to extract and insert function parameters
bool _extract_parameters(
		struct function_t* function,
		struct token_array_t* token_buffer,
		size_t* token_buffer_idx,
		struct err_msg_t* err)
{
	// TODO: add support for references (&x vs x)
	
	struct variable_t variable;
	struct parameter_t parameter;

	bool success = true;
	bool variable_created = false;

	// keep reading argument list until closing parenthesis
	while (*token_buffer_idx < token_buffer->length
			&& token_buffer->token[*token_buffer_idx].type != CLOSE_PAREN)
	{
		// when encountering data type, create new variable
		if (token_buffer->token[*token_buffer_idx].category == DATATYPE)
		{
			variable_create(&variable);
			variable_created = true;
			
			success = variable_set_type(
					&variable, 
					token_buffer->token[*token_buffer_idx].type,
					err);

			if (!success)
				return false;

			(*token_buffer_idx)++;
		}

		else if (token_buffer->token[*token_buffer_idx].type == TEXT)
		{
			success = variable_set_name(
					&variable,
					token_buffer->token[*token_buffer_idx].text,
					err);

			if (!success)
				return false;

			(*token_buffer_idx)++;
		}

		else if (token_buffer->token[*token_buffer_idx].type == COMMA)
		{
			parameter_create(&parameter, &variable);
			function_add_parameter(function, &parameter, err);
			variable_created = false;

			(*token_buffer_idx)++;
		}

		// TODO: add support for assignment (default parameter values)
	}

	// add last variable before closing parenthesis
	if (variable_created)
	{
		parameter_create(&parameter, &variable);
		function_add_parameter(function, &parameter, err);
	}

	return success;
}

bool _extract_variable_definition(
		struct function_t* function,
		struct token_array_t* function_buffer,
		struct err_msg_t* err)
{
	return parser_create_local_variable(function, function_buffer, err);
}

// declaration: int32 func(int32 x, int32 y);
// definition: int32 func(int32 x, int32 y) { ... }

bool parser_create_function(
		struct global_scope_t* global_scope,
		struct token_array_t* token_buffer,
		struct err_msg_t* err)
{
	size_t token_buffer_idx = 0;

	struct function_t function;
	bool success = function_create(&function, err);

	if (!success)
		return false;

	// first token will be return data type
	enum token_type_e return_type = token_buffer->token[token_buffer_idx++].type;
	success = function_set_return_type(&function, return_type, err);

	if (!success)
		return false;

	// second token will be name
	const char* function_name = token_buffer->token[token_buffer_idx++].text;
	success = function_set_name(&function, function_name, err);

	if (!success)
		return false;

	// third token will be '('
	// if fourth token is ')' then we have no parameters.
	// otherwise, we'll need to parse parameters
	token_buffer_idx++; // move past '('
	if (token_buffer->token[token_buffer_idx].type != CLOSE_PAREN)
		success = _extract_parameters(&function, token_buffer, &token_buffer_idx, err);
	
	token_buffer_idx++; // move past ')'

	// if we reach the end of token buffer after parsing parameters (if any)
	// then return here as it was only a declaration (is_defined = false is the default
	// so we don't need to explicitly set it)
	if (token_buffer_idx == token_buffer->length)
	{
		success = gscope_add_function(global_scope, &function, err);
		if (!success)
			return false;
		return true;
	}

	// otherwise, now we begin parsing the function body
	token_buffer_idx++; // move past '{'

	// this is a modified version of the main parser.c loop so there's 
	// a little bit of code dupe, but that's not a big deal
	
	struct token_array_t function_buffer;
	tkn_array_create(&function_buffer, 10);

	enum parse_definition_type definition_type;

	while (token_buffer_idx < token_buffer->length
			&& token_buffer->token[token_buffer_idx].type != CLOSE_BRACE)
	{

		printf("CURRENT TOKEN: %s\n", token_buffer->token[token_buffer_idx].text);

		switch (token_buffer->token[token_buffer_idx].category)
		{
			// defining variable in local scope
			case DATATYPE:
			{
				success = parse_definition(
						token_buffer,
						&token_buffer_idx,
						&function_buffer,
						&definition_type,
						false, // disallow defining functions inside functions
						err);

				if (!success)
					goto endparse;

				success = _extract_variable_definition(&function, &function_buffer, err);

				if (!success)
					goto endparse;

				// add instruction to initialize a variable to certain value
				const struct variable_t* variable = &function.variables[function.n_variables - 1];
				success = function_write_instruction(&function, INIT_VAR, variable->name, variable->value, err);
				if (!success)
					goto endparse;

				tkn_array_clear(&function_buffer);

				break;
			}

			default:
				break;
		}

		switch (token_buffer->token[token_buffer_idx].type)
		{
			// making function call (all tokens up to this point should be the function name)
			case OPEN_PAREN:
			{
				success = parser_create_function_call(
						global_scope, 
						&function, 
						token_buffer, 
						&token_buffer_idx,
						&function_buffer,
						err); 

				if (!success)
					goto endparse;

				break;
			}

			default:
				break;
		}
	}

endparse:
	if (!success)
	{
		tkn_array_free(&function_buffer);
		return false;
	}
	success = gscope_add_function(global_scope, &function, err);
	tkn_array_free(&function_buffer);
	return success;
}
