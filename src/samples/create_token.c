#include "token.h"
#include <stdio.h>

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
	struct token_t normal;
	tkn_create(&normal, "simple", 10, 5);
	print_token(&normal);
	
	struct token_t truncated;
	tkn_create(
		&truncated,
		"some extremely long super text that is very lengthy and I am getting pretty hungry",
		5,
		12);
	print_token(&truncated);

	return 0;
}
