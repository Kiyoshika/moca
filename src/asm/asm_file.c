#include "asm_file.h"

FILE* asm_open_file(const char* filename)
{
	FILE* asm_file = fopen(filename, "w+");
	return asm_file; // could be NULL!
}

void asm_close_file(FILE* asm_file)
{
	if (asm_file)
		fclose(asm_file);
}
