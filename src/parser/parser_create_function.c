#include "parser_create_function_return.h"
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
#include "built_in_functions.h"

bool _function_name_reserved(
		const char* function_name,
		struct err_msg_t* err)
{
	bool is_reserved = false;
	for (size_t i = 0; i < (size_t)N_BUILT_IN_FUNCTIONS; ++i)
	{
		if (strcmp(function_name, built_in_functions[i]) == 0)
		{
			is_reserved = true;
			err_write(err, "Cannot use a reserved function name.", 0, 0);
			break;
		}
	}

	return is_reserved;
}

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

	// read function name until '('  (space tokens are not included in the buffer)
	char new_function_name[FUNCTION_NAME_LEN];
	memset(new_function_name, 0, FUNCTION_NAME_LEN);
	size_t new_function_name_len = 0;

	while (token_buffer_idx < token_buffer->length
			&& token_buffer->token[token_buffer_idx].type != OPEN_PAREN)
	{
		char* new_name_token = token_buffer->token[token_buffer_idx].text;

		if (new_function_name_len + strlen(new_name_token) > FUNCTION_NAME_LEN - 1)
		{
			err_write(
				err,
				"Function name cannot exceed 50 characters.",
				token_buffer->token[token_buffer_idx].line_num,
				token_buffer->token[token_buffer_idx].char_pos);

			return false;
		}

		strncat(new_function_name, new_name_token, strlen(new_name_token));
		new_function_name_len += strlen(new_name_token);

		token_buffer_idx++;
	}

	if (_function_name_reserved(new_function_name, err))
		return false;

	success = function_set_name(&function, new_function_name, err);

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


		tkn_array_push(&function_buffer, &token_buffer->token[token_buffer_idx++]);
		switch (function_buffer.token[function_buffer.length - 1].category)
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
				continue;

				break; // unreachable
			}

			default:
				break;
		}

		switch (function_buffer.token[function_buffer.length - 1].type)
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

				// checking for semicolon after parsing function
				if (token_buffer_idx == token_buffer->length
					|| token_buffer->token[token_buffer_idx].type != END_STATEMENT)
				{
					err_write(err, "Missing semicolon after function.",
						token_buffer->token[token_buffer_idx].line_num,
						token_buffer->token[token_buffer_idx].char_pos);
					success = false;
					goto endparse;
				}

				token_buffer_idx++; // move past ';'
				tkn_array_clear(&function_buffer);

				break;
			}

			// return statement from function
			case RETURN:
			{
				success = parser_create_function_return(
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
