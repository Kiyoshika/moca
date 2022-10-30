#include "source_buffer.h"

bool srcbuffer_read(
	const char* filepath,
	struct source_buffer_t* source_buffer)
{
	FILE* src_file = fopen(filepath, "r");

	if (!src_file)
		return false;

	fseek(src_file, 0L, SEEK_END);
	source_buffer->length = ftell(src_file);
	source_buffer->buffer = calloc(source_buffer->length, sizeof(char));
	if (!source_buffer->buffer)
		return false;
	rewind(src_file);
	fread(source_buffer->buffer, sizeof(char), source_buffer->length, src_file);

	fclose(src_file);

	return true;
}

void srcbuffer_free(
	struct source_buffer_t* source_buffer)
{
	free(source_buffer->buffer);
	source_buffer->buffer = NULL;

	source_buffer->length = 0;
}
