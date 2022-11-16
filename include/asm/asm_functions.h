#ifndef ASM_FUNCTIONS_H
#define ASM_FUNCTIONS_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

// forward declaration
struct global_scope_t;

// create the .section .text to write functions
void asm_create_text_section(FILE* asm_file);

// write all functions from global scope into the assembly counterparts
void asm_function_create(
		FILE* asm_file,
		const struct global_scope_t* global_scope);

#endif
