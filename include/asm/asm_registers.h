#ifndef ASM_REGISTERS_H
#define ASM_REGISTERS_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

// get the stack pointer (64-bit version only)
void asm_get_rsp_register(char (*register_text)[5]);

// get the base pointer (64-bit version only)
void asm_get_rbp_register(char (*register_text)[5]);

// get the instruction pointer (64-bit version only)
void asm_get_rip_register(char (*register_text)[5]);

// get the corresponding rax register depending on size (1, 2, 4, 8 bytes)
void asm_get_rax_register(char (*register_text)[5], size_t size);

// get the corresponding rbx register depending on size (1, 2, 4, 8 bytes)
void asm_get_rbx_register(char (*register_text)[5], size_t size);

// get the corresponding rcx register depending on size (1, 2, 4, 8 bytes)
void asm_get_rcx_register(char (*register_text)[5], size_t size);

// get the corresponding rdx register depending on size (1, 2, 4, 8 bytes)
void asm_get_rdx_register(char (*register_text)[5], size_t size);

// get the corresponding rdi register depending on size (1, 2, 4, 8 bytes)
void asm_get_rdi_register(char (*register_text)[5], size_t size);

// get the corresponding rsi register depending on size (1, 2, 4, 8 bytes)
void asm_get_rsi_register(char (*register_text)[5], size_t size);

// get the corresponding r8 register depending on size (1, 2, 4, 8 bytes)
void asm_get_r8_register(char (*register_text)[5], size_t size);

// get the corresponding r9 register depending on size (1, 2, 4, 8 bytes)
void asm_get_r9_register(char (*register_text)[5], size_t size);

#endif
