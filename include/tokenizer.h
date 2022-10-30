#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "token_array.h"
#include "source_buffer.h"
#include <ctype.h>

// Extract tokens from [source_buffer] and store them into [token_array].
// [token_array] will be cleared before extracting tokens.
void tknzer_extract_tokens(
	struct token_array_t* token_array,
	const struct source_buffer_t* source_buffer);

#endif
