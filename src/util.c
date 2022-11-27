#include "util.h"
#include "token_array.h"
#include "token.h"
#include "err_msg.h"

static bool _text_realloc(
		char** text,
		size_t* text_capacity)
{
	void* alloc = realloc(*text, *text_capacity * 2);
	if (!alloc)
		return false;

	*text = alloc;
	*text_capacity *= 2;

	return true;
}

char* util_get_text_between_quotes(
		struct token_array_t* token_buffer,
		size_t* buffer_idx,
		struct err_msg_t* err)
{
	// start off with 100 bytes, but may need to reallocate more depending on how large the string is
	size_t text_len = 0;
	size_t text_capacity = 100;
	char* text = malloc(text_capacity);

	if (!text)
	{
		err_write(err, "Couldn't allocate memory for string.", 0, 0);
		return NULL;
	}

	memset(text, 0, text_capacity);
	text[0] = '"'; // start text value with single quote
	text_len++;

	// iterate everything in between double quotes
	// NOTE: it's also possible that there is no ending quote which is an error,
	// we will check for this
	bool contains_end_quote = false;
	while (*buffer_idx < token_buffer->length)
	{
		if (token_buffer->token[*buffer_idx].type == DOUBLE_QUOTE)
		{
			contains_end_quote = true;
			if (text_len + 2 >= text_capacity)
			{
				if (!_text_realloc(&text, &text_capacity))
				{
					err_write(err, "Couldn't allocate memory for string.", 0, 0);
					free(text);
					return NULL;
				}
			}
			
			text[text_len++] = '"';
			//text_len++;
			break;
		}

		char* new_text = token_buffer->token[*buffer_idx].text;
		size_t new_text_len = strlen(new_text);

		if (text_len + new_text_len >= text_capacity)
		{
			if (!_text_realloc(&text, &text_capacity))
			{
				err_write(err, "Couldn't allocate memory for string.", 0, 0);
				free(text);
				return NULL;
			}
		}

		strncat(text, new_text, new_text_len);
		text_len += new_text_len;

		(*buffer_idx)++;
	}

	if (!contains_end_quote)
	{
		err_write(err, "Missing end quote for string.", 0, 0);
		free(text);
		return NULL;
	}
	
	return text;
}
