#ifndef TOKEN_H
#define TOKEN_H

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

struct token_t
{
	char text[61]; 		// text content of the token
	size_t text_len;	// length of the text of the token
	size_t line_num; 	// line number token appears on; ranges from [1-inf]
	size_t char_pos; 	// character position on a given line number (line_num) [0-60]
};

// Create a new token with [token_text] on a given [line_num] at position [char_pos]
// with respect to the current [line_num].
// Returns true if token was created successfully, false otherwise.
void tkn_create(
	struct token_t* token,
	const char* token_text,
	const size_t line_num,
	const size_t char_pos);

#endif
