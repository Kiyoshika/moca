#include "tokenizer.h"

// split tokens on non-alpha/numeric characters
static inline bool is_tokenize_char(const char tokenize_char)
{
	return isalnum(tokenize_char) == 0;
}

void tknzer_extract_tokens(
	struct token_array_t* token_array,
	const struct source_buffer_t* source_buffer)
{
	// start off with generous 100 tokens (will be dynamically sized
	// during push if necessary)
	tkn_array_create(token_array, 100);

	char current_buff[MAX_LINE_LEN];
	size_t current_buff_idx = 0;
	memset(current_buff, 0, MAX_LINE_LEN);
	
	// int32 main()\n
	// {\n
	struct token_t current_token;

	for (size_t current_line_num = 0; current_line_num < source_buffer->n_lines; ++current_line_num)
	{
		for (size_t current_char = 0; current_char < strlen(source_buffer->line[current_line_num]); ++current_char)
		{
			const char char_value = source_buffer->line[current_line_num][current_char];
			if (is_tokenize_char(char_value) && current_buff_idx > 0) // only tokenize if buffer is non-empty
			{
				tkn_create(&current_token, current_buff, current_line_num + 1, current_char - strlen(current_buff) + 1);
				tkn_array_push(token_array, &current_token);
				memset(current_buff, 0, MAX_LINE_LEN);
				current_buff_idx = 0;
			}
		
			// ignore these characters - don't add to buffer	
			if (char_value != '\n' && char_value != ' ' && char_value != '\t')
				current_buff[current_buff_idx++] = char_value; 
		}
	}
}
