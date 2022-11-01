#include "tokenizer.h"

void print_token(const struct token_t* token)
{
	printf("TOKEN TEXT:\t%s\n", token->text);
	printf("TOKEN LEN:\t%zu\n", token->text_len);
	printf("TOKEN LINE NUM:\t%zu\n", token->line_num);
	printf("TOKEN CHAR POS:\t%zu\n", token->char_pos);
	printf("\n");
}

int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./mainfile.moca", &srcbuffer);
	
	struct token_array_t array;
	tknzer_extract_tokens(&array, &srcbuffer);

	for (size_t i = 0; i < array.length; ++i)
		print_token(&array.token[i]);

	tkn_array_free(&array);
	srcbuffer_free(&srcbuffer);
	return 0;
}
