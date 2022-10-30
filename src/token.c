#include "token.h"

void tkn_create(
	struct token_t* token,
	const char* token_text,
	const size_t line_num,
	const size_t char_pos)
{
	// ensure text doesn't surpass 60 chars
	size_t text_len = strlen(token_text);
	text_len = text_len > 60 ? 60 : text_len;
	
	token->text_len = text_len;
	token->line_num = line_num;
	token->char_pos = char_pos;
	memcpy(&token->text[0], token_text, text_len);
	token->text[text_len] = '\0';
}
