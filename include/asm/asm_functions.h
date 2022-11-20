#ifndef ASM_FUNCTIONS_H
#define ASM_FUNCTIONS_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

// forward declaration
struct global_scope_t;
struct err_msg_t;

// create the .section .text to write functions
void asm_create_text_section(FILE* asm_file);

// write all functions from global scope into the assembly counterparts
bool asm_function_create(
		FILE* asm_file,
		struct global_scope_t* global_scope,
		struct err_msg_t* err);

#endif
