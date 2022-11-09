#include "parser.h"
#include "lexer.h"
#include "source_buffer.h"
#include "tokenizer.h"
#include "token_array.h"
#include "err_msg.h"
#include "global_scope.h"

int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./missing_end_statement.moca", &srcbuffer);

	struct token_array_t token_array;
	tknzer_extract_tokens(&token_array, &srcbuffer);

	lexer_parse(&token_array);

	struct global_scope_t global_scope;
	struct err_msg_t err;
	bool parsed = parse_tokens(&token_array, &global_scope, &err);

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&token_array);

	int testval = -1;

	if (!parsed)
	{
		const char* expected_msg = "Missing end statement (semicolon).";

		if (strcmp(expected_msg, err.msg) != 0)
		{
			fprintf(stderr, "Wrong error message. Expected '%s' - got '%s'\n", expected_msg, err.msg);
			testval = -1;
		}
		else if (err.line_num != 1)
		{
			fprintf(stderr, "Expected error to be on line %zu - got %zu\n", (size_t)1, err.line_num);
			testval = -1;
		}
		else if (err.char_pos != 7)
		{
			fprintf(stderr, "Expected error to be at position %zu - got %zu\n", (size_t)7, err.char_pos);
			testval = -1;
		}
		else
			testval = 0;
	}
	else
		fprintf(stderr, "Was expecting a 'missing end statement' error but parser didn't identify it.\n");

	return testval;
}
