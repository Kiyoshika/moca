#include "parser.h"
#include "source_buffer.h"
#include "token_array.h"
#include "token.h"
#include "global_scope.h"
#include "tokenizer.h"
#include "variables.h"
#include "lexer.h"
#include "err_msg.h"

size_t current_var = 1;

static int validate_variable(
		const struct variable_t* variable,
		enum token_type_e expected_type,
		size_t expected_bytes_size,
		int64_t expected_value) // int64_t is large enough to hold all (signed) types
{
	if (variable->type != expected_type)
	{
		fprintf(stderr, "Variable #%zu: Expected type '%s' but got '%s'.\n", current_var, type_to_text(variable->type), type_to_text(expected_type));
		return -1;
	}

	if (variable->bytes_size != expected_bytes_size)
	{
		fprintf(stderr, "Variable #%zu: Expected size (bytes) '%zu' but got '%zu'.\n", current_var, expected_bytes_size, variable->bytes_size);
		return -1;
	}

	switch (variable->type)
	{
		case INT8:
		{
			int8_t val = *(int8_t*)variable->value;
			if (val != expected_value)
			{
				fprintf(stderr, "Variable #%zu: Expected value '%ld' but got '%d'.\n", current_var, expected_value, val);
				return -1;
			}
			break;
		}

		case INT16:
		{
			int16_t val = *(int16_t*)variable->value;
			if (val != expected_value)
			{
				fprintf(stderr, "Variable #%zu: Expected value '%ld' but got '%d'.\n", current_var, expected_value, val);
				return -1;
			}
			break;
		}

		case INT32:
		{
			int32_t val = *(int32_t*)variable->value;	
			if (val != expected_value)
			{
				fprintf(stderr, "Variable #%zu: Expected value '%ld' but got '%d'.\n", current_var, expected_value, val);
				return -1;
			}
			break;
		}

		case INT64:
		{
			int64_t val = *(int64_t*)variable->value;	
			if (val != expected_value)
			{
				fprintf(stderr, "Variable #%zu: Expected value '%ld' but got '%ld'.\n", current_var, expected_value, val);
				return -1;
			}
			break;
		}

		default:
		{
			fprintf(stderr, "Variable: #%zu: Unknown type.", current_var);
			return -1;
		}
	}

	current_var++;

	return 0;
}

int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./create_global_variables.moca", &srcbuffer);

	struct token_array_t token_array;
	tknzer_extract_tokens(&token_array, &srcbuffer);

	lexer_parse(&token_array);

	struct global_scope_t global_scope;
	struct err_msg_t err;
	bool parsed = parse_tokens(&token_array, &global_scope, &err);

	int testval = 0;

	if (!parsed)
	{
		fprintf(stderr, "[Line %zu, Pos %zu] Couldn't parse with error message: %s\n", err.line_num, err.char_pos, err.msg);
		testval = -1;
	}

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&token_array);

	struct variable_t* variable = &global_scope.variables[0];
	testval = validate_variable(variable, INT8, 1, 12);
	if (testval == -1) return testval;

	variable = &global_scope.variables[1];
	testval = validate_variable(variable, INT8, 1, -12);
	if (testval == -1) return testval;

	return testval;
}
