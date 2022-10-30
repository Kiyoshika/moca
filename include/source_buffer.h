#ifndef SOURCE_BUFFER_H 
#define SOURCE_BUFFER_H 

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LEN 251

struct source_buffer_t
{
	char (*line)[MAX_LINE_LEN];
	size_t n_lines;
};

// Pass a source_buffer_t by address and read a file
// to store the buffer contents.
// Returns true on success or false on failure to read file.
bool srcbuffer_read(
	const char* filepath,
	struct source_buffer_t* source_buffer);

// Deallocate memory held by a source buffer.
void srcbuffer_free(
	struct source_buffer_t* source_buffer);

#endif
