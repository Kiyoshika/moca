#include "parser.h"
#include "lexer.h"
#include "source_buffer.h"
#include "tokenizer.h"
#include "token_array.h"
#include "err_msg.h"

int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./too_many_datatype_tokens.moca", &srcbuffer);

	struct token_array_t token_array;
	tknzer_extract_tokens(&token_array, &srcbuffer);

	lexer_parse(&token_array);

	struct err_msg_t err;
	parse_tokens(&token_array, &err);

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&token_array);

	int testval = 0;
	char* expected_msg = "Too many datatype tokens.";
	char* actual_msg = err.msg;
	// check correct error message
	if (strcmp(err.msg, expected_msg) != 0)
	{
		fprintf(stderr, "Expected message '%s' - got '%s'\n", expected_msg, actual_msg);
		testval = -1;
	}
	// check correct line number
	if (err.line_num != 1)
	{
		fprintf(stderr, "Expected line number '%zu' - got '%zu'\n", (size_t)1, err.line_num);
		testval = -1;
	}
	// check correct character position
	if (err.char_pos != 7)
	{
		fprintf(stderr, "Expected char position '%zu' - got '%zu'\n", (size_t)7, err.char_pos);
		testval = -1;
	}

	return testval;
}
