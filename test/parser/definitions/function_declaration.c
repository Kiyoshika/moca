#include "parser.h"
#include "lexer.h"
#include "source_buffer.h"
#include "tokenizer.h"
#include "token_array.h"
#include "err_msg.h"
#include "global_scope.h"
#include "parameters.h"
#include "variables.h"
#include "functions.h"

int validate_param(
		const struct parameter_t* parameter,
		const size_t param_num,
		bool expected_is_reference,
		const enum token_type_e expected_type,
		const char* expected_name)
{
	int testval = 0;

	if (parameter->is_reference != expected_is_reference)
	{
		if (!expected_is_reference)
			fprintf(stderr, "Parameter #%zu: Parameter is labelled as a reference when it shouldn't be.\n", param_num);
		else
			fprintf(stderr, "Parameter #%zu: Parameter is not labelled as a reference when it should be.\n", param_num);

		testval = -1;
	}

	const struct variable_t* variable = &parameter->variable;

	if (variable->type != expected_type)
	{
		fprintf(stderr, "Parameter #%zu: Should be '%s' but is '%s'.\n", param_num, type_to_text(expected_type), type_to_text(variable->type));
		testval = -1;
	}

	if (strcmp(variable->name, expected_name) != 0)
	{
		fprintf(stderr, "Parameter #%zu: Should have name '%s' but is '%s'.\n", param_num, variable->name, expected_name);
		testval = -1;
	}

	return testval;
}

int validate_param_count(
		const size_t current_function,
		const size_t n_parameters,
		const size_t expected_parameters)
{
	if (n_parameters != expected_parameters)
	{
		fprintf(stderr, "Function %zu: Should have %zu parameters but has %zu.\n", current_function, expected_parameters, n_parameters);
		return -1;
	}

	return 0;
}

#define N_FUNCTIONS 2
#define MAX_PARAMS 2

int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./function_declaration.moca", &srcbuffer);

	struct token_array_t token_array;
	tknzer_extract_tokens(&token_array, &srcbuffer);

	lexer_parse(&token_array);

	struct global_scope_t global_scope;
	struct err_msg_t err;
	bool parsed = parse_tokens(&token_array, &global_scope, &err);

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&token_array);

	int testval = 0;

	if (!parsed)
	{
		fprintf(stderr, "[Line %zu, Pos %zu] Couldn't parse with error message: %s\n", err.line_num, err.char_pos, err.msg);
		testval = -1;
	}

	// check correct function counts
	if (global_scope.n_functions != N_FUNCTIONS)
	{
		fprintf(stderr, "Expected %zu functions but have %zu.\n", (size_t)N_FUNCTIONS, global_scope.n_functions);
		testval = -1;
	}

	// check correct parameter counts
	const size_t expected_param_counts[N_FUNCTIONS] = {
		0,
		2
	};

	for (size_t i = 0; i < global_scope.n_functions; ++i)
	{
		int valid = validate_param_count(i + 1, global_scope.functions[i].n_parameters, expected_param_counts[i]);
		testval = valid == -1 ? -1 : testval;
	}

	// validate parameters
	const bool expected_is_reference[N_FUNCTIONS][MAX_PARAMS] = {
		{ false, false },
		{ false, false }
	};

	const enum token_type_e expected_type[N_FUNCTIONS][MAX_PARAMS] = {
		{ NONE, NONE },
		{ INT32, INT32 }
	};

	const char* expected_name[N_FUNCTIONS][MAX_PARAMS] = {
		{ "", "" },
		{ "x", "y" }
	};

	for (size_t i = 0; i < global_scope.n_functions; ++i)
	{
		for (size_t k = 0; k < global_scope.functions[i].n_parameters; ++k)
		{
			int valid = validate_param(
					&global_scope.functions[i].parameters[k],
					k + 1,
					expected_is_reference[i][k],
					expected_type[i][k],
					expected_name[i][k]);
			testval = valid == -1 ? -1 : testval;
		}
	}

	gscope_free(&global_scope);

	return testval;
}
