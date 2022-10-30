#include "source_buffer.h"

int main()
{
	struct source_buffer_t srcbuffer;
	srcbuffer_read("./buffer.moca", &srcbuffer);

	printf("BUFFER LENGTH: %zu\n", srcbuffer.length);
	printf("\nBUFFER CONTENTS: %s\n", srcbuffer.buffer);

	srcbuffer_free(&srcbuffer);

	return 0;
}
