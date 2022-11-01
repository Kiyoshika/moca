#include "token_array.h"
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
	struct token_t one, two, three;
	tkn_create(&one, "one", 1, 1);
	tkn_create(&two, "two", 2, 2);
	tkn_create(&three, "three", 3, 3);

	struct token_array_t array;
	tkn_array_create(&array, 3);

	tkn_array_push(&array, &one);
	tkn_array_push(&array, &two);
	tkn_array_push(&array, &three);

	for (size_t i = 0; i < 3; ++i)
		print_token(&array.token[i]);

	tkn_array_free(&array);

	return 0;
}
