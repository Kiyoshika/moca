#include "asm_sections.h"

void asm_create_text_section(FILE* asm_file)
{
	fprintf(asm_file, "%s\n", ".section .text");
	fprintf(asm_file, "%s\n", ".globl main");
}

void asm_create_data_section(FILE* asm_file)
{
	fprintf(asm_file, "%s\n", ".section .data");
}
