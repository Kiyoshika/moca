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
	srcbuffer_read("./variable_definition.moca", &srcbuffer);

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

	return testval;
}
