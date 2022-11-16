#include "asm_functions.h"
#include "asm_file.h"
#include "functions.h"
#include "token.h"
#include "tokenizer.h"
#include "global_scope.h"
#include "token_array.h"
#include "lexer.h"
#include "err_msg.h"
#include "parser.h"

#include <stdio.h>

int main()
{
	FILE* asm_file = asm_open_file("sample.s");
	if (!asm_file)
	{
		fprintf(stderr, "Problem opening file to write assembly.\n");
		return -1;
	}

	struct source_buffer_t srcbuffer;
	srcbuffer_read("./sample.moca", &srcbuffer);

	struct token_array_t token_array;
	tknzer_extract_tokens(&token_array, &srcbuffer);

	lexer_parse(&token_array);

	struct global_scope_t global_scope;
	struct err_msg_t err;
	parse_tokens(&token_array, &global_scope, &err);

	asm_create_text_section(asm_file);
	asm_function_create(asm_file, &global_scope);

	srcbuffer_free(&srcbuffer);
	tkn_array_free(&token_array);
	gscope_free(&global_scope);

	asm_close_file(asm_file);

	return 0;
}
