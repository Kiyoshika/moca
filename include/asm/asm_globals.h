#ifndef ASM_GLOBALS_H
#define ASM_GLOBALS_H

#include <stdio.h>
#include <stdbool.h>

// forward declarations
struct global_scope_t;

bool asm_write_global_variables(
	FILE* asm_file,
	const struct global_scope_t* global_scope);

#endif
