#include "source_buffer.h"

bool srcbuffer_read(
	const char* filepath,
	struct source_buffer_t* source_buffer)
{
	FILE* src_file = fopen(filepath, "r");

	if (!src_file)
		return false;

	// count total lines in file
	char current_line[MAX_LINE_LEN];
	source_buffer->n_lines = 0;

	while (fgets(current_line, MAX_LINE_LEN, src_file) != NULL)
		source_buffer->n_lines++;

	rewind(src_file); // reset file pointer back to beginning
	
	source_buffer->line = calloc(source_buffer->n_lines, MAX_LINE_LEN);

	// read lines into each line buffer
	size_t current_line_num = 0;
	while (fgets(current_line, MAX_LINE_LEN, src_file) != NULL)
		memcpy(&source_buffer->line[current_line_num++][0], current_line, strlen(current_line));

	fclose(src_file);

	return true;
}

void srcbuffer_free(
	struct source_buffer_t* source_buffer)
{
	free(source_buffer->line);
	source_buffer->line= NULL;

	source_buffer->n_lines = 0;
}
