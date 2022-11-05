#include "tokenizer.h"
#include "tokenizer_internal.c"

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
	
	struct token_t current_token;

	for (size_t current_line_num = 0; current_line_num < source_buffer->n_lines; ++current_line_num)
	{
		for (size_t current_char = 0; current_char < strlen(source_buffer->line[current_line_num]); ++current_char)
		{
			const char char_value = source_buffer->line[current_line_num][current_char];
			char next_char_value = 0;
			if (current_char < strlen(source_buffer->line[current_line_num]) - 1)
				next_char_value = source_buffer->line[current_line_num][current_char + 1];

			// parse double tokens (&&, +=, !=, etc.)
			if (current_char + 1 < strlen(source_buffer->line[current_line_num])
					&& is_double_token(char_value, next_char_value))
			{
				// if buffer already contains contents, tokenize that first.
				// e.g., "z++" -> {z, ++}
				if (strlen(current_buff) > 0)
				{
					push_into_token_array(
						&current_token,
						token_array,
						current_buff,
						current_line_num,
						current_char,
						&current_buff_idx);
				}

				current_buff[current_buff_idx++] = char_value;
				current_buff[current_buff_idx++] = next_char_value;
				current_char++;				

				push_into_token_array(
						&current_token,
						token_array,
						current_buff,
						current_line_num,
						current_char,
						&current_buff_idx);

			}

			// tokenize whatever's currently in char buffer whenever hitting non alphanumeric characters.
			// the only exception cases are decimals that are tied to a number (e.g., .33 or 5.63)
			else if (is_tokenize_char(char_value)
						&& is_not_decimal(char_value, next_char_value))
			{
				if (strlen(current_buff) > 0)
				{
					push_into_token_array(
						&current_token,
						token_array,
						current_buff,
						current_line_num,
						current_char,
						&current_buff_idx);
				}

				if (!ignore_char(char_value))
				{
					current_buff[current_buff_idx++] = char_value;
					push_into_token_array(
						&current_token,
						token_array,
						current_buff,
						current_line_num,
						current_char,
						&current_buff_idx);

				}
				
			}
		
			else if (!ignore_char(char_value))
				current_buff[current_buff_idx++] = char_value; 
		}
	}
}
