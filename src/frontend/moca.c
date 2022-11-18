#include <stdio.h>
#include <string.h>

#include "source_buffer.h"
#include "token.h"
#include "tokenizer.h"
#include "token_array.h"
#include "parser.h"
#include "lexer.h"
#include "functions.h"
#include "variables.h"
#include "asm_functions.h"
#include "asm_file.h"
#include "err_msg.h"
#include "global_scope.h"

/*
 * This is the frontend for the moca compiler.
 * 
 * At the moment, it only supports compiling single files
 * and ONLY generates the assembly (does not yet call gcc).
 */
int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Please provide a filename.\n");
		return -1;
	}

	// moca -v
	// moca -version
	if (strncmp(argv[1], "-v", 2) == 0
		|| strncmp(argv[1], "-version", 8) == 0)
	{
		printf("moca version 0.1\n");
		return 0;
	}

	// RUN THE COMPILER (put this in its own function eventually, but
	// for now can just put it here...)
	
	// argv[1] = filename
	
	// TODO: validate file extension

	struct source_buffer_t srcbuffer;
	srcbuffer_read(argv[1], &srcbuffer);

	struct token_array_t token_array;
	tknzer_extract_tokens(&token_array, &srcbuffer);

	lexer_parse(&token_array);

	struct global_scope_t global_scope;
	struct err_msg_t err;
	bool parsed = parse_tokens(&token_array, &global_scope, &err);
	if (!parsed)
	{
		err_print(&err);
		goto endparse;
	}
	
	// change filename from [name].moca to [name].s
	char asm_file_name[51];
	memset(asm_file_name, 0, 51);
	size_t i = 0;
	for (; i < strlen(argv[1]); ++i)
	{
		if (argv[1][i] == '.' || i == 48) break;
		asm_file_name[i] = argv[1][i];
	}
	asm_file_name[i++] = '.';
	asm_file_name[i] = 's';

	FILE* asm_file = asm_open_file(asm_file_name);
	if (!asm_file)
	{
		printf("Problem opening file to write assembly.\n");
		return -1;
	}
	asm_create_text_section(asm_file);
	bool success = asm_function_create(asm_file, &global_scope, &err);
	// print error and cleanup
	if (!success)
		err_print(&err);
	asm_close_file(asm_file);

// cleanup memory even if parsing failed
endparse:
	srcbuffer_free(&srcbuffer);
	tkn_array_free(&token_array);
	gscope_free(&global_scope);
	
	return 0;
}
