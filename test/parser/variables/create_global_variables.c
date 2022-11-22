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
		char* expected_name,
		char* expected_value)
{
	int valid = 0;
	if (variable->type != expected_type)
	{
		fprintf(stderr, "Variable #%zu: Expected type '%s' but got '%s'.\n", current_var, type_to_text(variable->type), type_to_text(expected_type));
		valid = -1;
	}

	if (variable->bytes_size != expected_bytes_size)
	{
		fprintf(stderr, "Variable #%zu: Expected size (bytes) '%zu' but got '%zu'.\n", current_var, expected_bytes_size, variable->bytes_size);
		valid = -1;
	}

	if (strcmp(variable->name, expected_name) != 0)
	{
		fprintf(stderr, "Variable #%zu: Expected name '%s' but got '%s'.\n", current_var, expected_name, variable->name);
		valid = -1;
	}

	if (strcmp(variable->value, expected_value) != 0)
	{
		fprintf(stderr, "Variable #%zu: Expected value '%s' but got '%s'.\n", current_var, expected_value, variable->value);
		valid = -1;
	}

	current_var++;

	return valid;
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

	size_t expected_global_variables = 4;
	if (global_scope.n_variables != expected_global_variables)
	{
		fprintf(stderr, "Expected %zu global variables but got %zu.\n",
			expected_global_variables,
			global_scope.n_variables);

		testval = -1;
	}
	else
	{
		struct variable_t* variable = &global_scope.variables[0];
		int valid;
		valid = validate_variable(variable, INT8, 1, "x00", "12");
		testval = valid == -1 ? -1 : testval;

		variable = &global_scope.variables[1];
		valid = validate_variable(variable, INT8, 1, "x01", "-12");
		testval = valid == -1 ? -1 : testval;

		variable = &global_scope.variables[2];
		valid = validate_variable(variable, INT32, 4, "some_long_23_name", "100");
		testval = valid == -1 ? -1 : testval;

		variable = &global_scope.variables[3];
		valid = validate_variable(variable, INT32, 4, "some_other_long_name", "0");
		testval = valid == -1 ? -1 : testval;
	}
	
	gscope_free(&global_scope);

	return testval;
}
