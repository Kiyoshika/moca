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
	srcbuffer_read("./strings.moca", &srcbuffer);

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
	int valid;
	char* expected_str = "\"some string with some text\"";
	valid = validate_variable(variable, STRING, strlen(expected_str), expected_str);
	testval = valid == -1 ? -1 : testval;

	gscope_free(&global_scope);

	return testval;
}
