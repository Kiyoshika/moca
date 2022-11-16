#ifndef ASM_FILE_H
#define ASM_FILE_H

#include <stdlib.h>
#include <stdio.h>

// open a file to write assembly and return a file pointer.
// returns NULL on failure.
FILE* asm_open_file(const char* filename);

// close a file when finished writing assembly
void asm_close_file(FILE* asm_file);

#endif
