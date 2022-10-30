#include "source_buffer.h"

int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./buffer.moca", &srcbuffer);

	printf("TOTAL LINES: %zu\n\n", srcbuffer.n_lines);
	for (size_t i = 0; i < srcbuffer.n_lines; ++i)
		printf("%zu: %s\n", i+1, srcbuffer.line[i]);

	srcbuffer_free(&srcbuffer);

	return 0;
}
