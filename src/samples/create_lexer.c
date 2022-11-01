#include "lexer.h"
#include "token_array.h"
#include "tokenizer.h"

void print_token(const struct token_t* token)
{
	printf("TEXT: %s\n", token->text);
	printf("TYPE: %d\n", token->type);
	printf("CATEGORY: %d\n", token->category);
	printf("\n");
}

int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./mainfile.moca", &srcbuffer);
	
	struct token_array_t array;
	tknzer_extract_tokens(&array, &srcbuffer);

	lexer_parse(&array);

	for (size_t i = 0; i < array.length; ++i)
		print_token(&array.token[i]);

	tkn_array_free(&array);
	srcbuffer_free(&srcbuffer);

	return 0;
}
