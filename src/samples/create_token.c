#include "token.h"
#include <stdio.h>

int main()
{
	struct token_t normal;
	tkn_create(&normal, "simple", 10, 5);
	printf("TOKEN TEXT:\t%s\n", normal.text);
	printf("TOKEN LEN:\t%zu\n", normal.text_len);
	printf("TOKEN LINE NUM:\t%zu\n", normal.line_num);
	printf("TOKEN CHAR POS:\t%zu\n", normal.char_pos);
	printf("\n");

	struct token_t truncated;
	tkn_create(
		&truncated,
		"some extremely long super text that is very lengthy and I am getting pretty hungry",
		5,
		12);
	printf("TOKEN TEXT:\t%s\n", truncated.text);
	printf("TOKEN LEN:\t%zu\n", truncated.text_len);
	printf("TOKEN LINE NUM:\t%zu\n", truncated.line_num);
	printf("TOKEN CHAR POS:\t%zu\n", truncated.char_pos);


	return 0;
}
