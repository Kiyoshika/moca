#include "parser.h"
#include "lexer.h"
#include "source_buffer.h"
#include "tokenizer.h"
#include "token_array.h"
#include "err_msg.h"
#include "global_scope.h"
#include "variables.h"
#include "functions.h"

int validate_variable(
		const size_t function_n,
		const size_t variable_n,
		const struct variable_t* variable,
		const enum token_type_e expected_type,
		const char* expected_name,
		const char* expected_value,
		const size_t expected_bytes_size,
		const bool expected_is_initialized,
		const bool expected_is_signed)
{
	int testval = 0;

	if (expected_type != variable->type)
	{
		fprintf(stderr, "[Function: %zu, Variable: %zu] Expected type '%s' but got '%s'.\n",
				function_n,
				variable_n,
				type_to_text(expected_type),
				type_to_text(variable->type));
		testval = -1;
	}

	if (strcmp(expected_name, variable->name) != 0)
	{
		fprintf(stderr, "[Function: %zu, Variable: %zu] Expected name '%s' but got '%s'.\n",
				function_n,
				variable_n,
				expected_name,
				variable->name);
		testval = -1;
	}

	if (strcmp(expected_value, variable->value) != 0)
	{
		fprintf(stderr, "[Function: %zu, Variable: %zu] Expected value '%s' but got '%s'.\n",
				function_n,
				variable_n,
				expected_value,
				variable->value);
		testval = -1;
	}

	if (expected_bytes_size != variable->bytes_size)
	{
		fprintf(stderr, "[Function: %zu, Variable: %zu] Expected size '%zu' but got '%zu'.\n",
				function_n,
				variable_n,
				expected_bytes_size,
				variable->bytes_size);
		testval = -1;
	}

	if (expected_is_initialized != variable->is_initialized)
	{
		fprintf(stderr, "[Function: %zu, Variable: %zu] Expected is_initalized to be '%s' but got '%s'.\n",
				function_n,
				variable_n,
				expected_is_initialized ? "true" : "false",
				variable->is_initialized ? "true" : "false");
		testval = -1;
	}

	if (expected_is_signed != variable->is_signed)
	{
		fprintf(stderr, "[Function: %zu, Variable: %zu] Expected is_signed to be '%s' but got '%s'.\n",
				function_n,
				variable_n,
				expected_is_signed ? "true" : "false",
				variable->is_signed ? "true" : "false");
		testval = -1;
	}

	return testval;
}

int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./function_definition.moca", &srcbuffer);

	struct token_array_t token_array;
	tknzer_extract_tokens(&token_array, &srcbuffer);

	lexer_parse(&token_array);

	struct global_scope_t global_scope;
	struct err_msg_t err;
	if (!gscope_create(&global_scope, &err))
	{
		fprintf(stderr, "Failed to create global scope.\n");
		return -1;
	}
	bool parsed = parse_tokens(&token_array, &global_scope, &err);

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&token_array);

	int testval = 0;

	if (!parsed)
	{
		fprintf(stderr, "[Line %zu, Pos %zu] Couldn't parse with error message: %s\n", err.line_num, err.char_pos, err.msg);
		testval = -1;
	}


	// two functions with two local variables each (to make it easier to read tests)
	#define N_FUNCTIONS 2
	#define N_VARIABLES 2

	const enum token_type_e expected_type[N_FUNCTIONS][N_VARIABLES] = {
		{ INT32, INT32 },
		{ INT64, INT64 }
	};

	const char* expected_name[N_FUNCTIONS][N_VARIABLES] = {
		{ "z", "w" },
		{ "q", "z" }
	};

	const char* expected_value[N_FUNCTIONS][N_VARIABLES] = {
		{ "5", "y" },
		{ "1", "-12" }
	};

	const size_t expected_bytes_size[N_FUNCTIONS][N_VARIABLES] = {
		{ 4, 4 },
		{ 8, 8 }
	};

	const bool expected_is_initialized[N_FUNCTIONS][N_VARIABLES] = {
		{ true, true },
		{ true, true }
	};

	const bool expected_is_signed[N_FUNCTIONS][N_VARIABLES] = {
		{ true, true },
		{ true, true }
	};

	// check correct # of functions
	if (global_scope.n_functions != N_FUNCTIONS)
	{
		fprintf(stderr, "Was expecting %zu functions but got %zu.\n", (size_t)N_FUNCTIONS, global_scope.n_functions);
		testval = -1;
		goto earlystop;
	}

	for (size_t n_func = 0; n_func < global_scope.n_functions; ++n_func)
	{
		// check correct # of variables
		if (global_scope.functions[n_func].n_variables != N_VARIABLES)
		{
			fprintf(stderr, "Was expecting %zu variables inside function #%zu but got %zu.\n",
					(size_t)N_VARIABLES,
					n_func + 1,
					global_scope.functions[n_func].n_variables);
			testval = -1;
			goto earlystop;
		}

		for (size_t n_var = 0; n_var < global_scope.functions[n_func].n_variables; ++n_var)
		{
			int tempval = validate_variable(
					n_func,
					n_var,
					&global_scope.functions[n_func].variables[n_var],
					expected_type[n_func][n_var],
					expected_name[n_func][n_var],
					expected_value[n_func][n_var],
					expected_bytes_size[n_func][n_var],
					expected_is_initialized[n_func][n_var],
					expected_is_signed[n_func][n_var]);
			testval = tempval == -1 ? -1 : testval;
		}
	}

earlystop:
	gscope_free(&global_scope);

	return testval;
}
