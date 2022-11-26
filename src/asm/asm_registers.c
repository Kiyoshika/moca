#include "asm_registers.h"

void asm_get_rsp_register(char (*register_text)[5])
{
	memset(register_text, 0, 5);
	memcpy(register_text, "%rsp\0", 5);
}

void asm_get_rbp_register(char (*register_text)[5])
{
	memset(register_text, 0, 5);
	memcpy(register_text, "%rbp\0", 5);
}

void asm_get_rip_register(char (*register_text)[5])
{
	memset(register_text, 0, 5);
	memcpy(register_text, "%rip\0", 5);
}

void asm_get_rax_register(char (*register_text)[5], size_t size)
{
	memset(register_text, 0, 5);

	switch (size)
	{
		case 1:
			memcpy(register_text, "%al\0", 4);
			break;
		case 2:
			memcpy(register_text, "%ax\0", 4);
			break;
		case 4:
			memcpy(register_text, "%eax\0", 5);
			break;
		case 8:
			memcpy(register_text, "%rax\0", 5);
			break;
		default:
			break;
	}
}

void asm_get_rbx_register(char (*register_text)[5], size_t size)
{
	memset(register_text, 0, 5);

	switch (size)
	{
		case 1:
			memcpy(register_text, "%bl\0", 4);
			break;
		case 2:
			memcpy(register_text, "%bx\0", 4);
			break;
		case 4:
			memcpy(register_text, "%ebx\0", 5);
			break;
		case 8:
			memcpy(register_text, "%rbx\0", 5);
			break;
		default:
			break;
	}
}

void asm_get_rcx_register(char (*register_text)[5], size_t size)
{
	memset(register_text, 0, 5);

	switch (size)
	{
		case 1:
			memcpy(register_text, "%cl\0", 4);
			break;
		case 2:
			memcpy(register_text, "%cx\0", 4);
			break;
		case 4:
			memcpy(register_text, "%ecx\0", 5);
			break;
		case 8:
			memcpy(register_text, "%rcx\0", 5);
			break;
		default:
			break;
	}
}

void asm_get_rdx_register(char (*register_text)[5], size_t size)
{
	memset(register_text, 0, 5);

	switch (size)
	{
		case 1:
			memcpy(register_text, "%dl\0", 4);
			break;
		case 2:
			memcpy(register_text, "%dx\0", 4);
			break;
		case 4:
			memcpy(register_text, "%edx\0", 5);
			break;
		case 8:
			memcpy(register_text, "%rdx\0", 5);
			break;
		default:
			break;
	}
}

void asm_get_rdi_register(char (*register_text)[5], size_t size)
{
	memset(register_text, 0, 5);

	switch (size)
	{
		case 1:
			memcpy(register_text, "%dil\0", 4);
			break;
		case 2:
			memcpy(register_text, "%di\0", 4);
			break;
		case 4:
			memcpy(register_text, "%edi\0", 5);
			break;
		case 8:
			memcpy(register_text, "%rdi\0", 5);
			break;
		default:
			break;
	}
}

void asm_get_rsi_register(char (*register_text)[5], size_t size)
{
	memset(register_text, 0, 5);

	switch (size)
	{
		case 1:
			memcpy(register_text, "%sil\0", 4);
			break;
		case 2:
			memcpy(register_text, "%si\0", 4);
			break;
		case 4:
			memcpy(register_text, "%esi\0", 5);
			break;
		case 8:
			memcpy(register_text, "%rsi\0", 5);
			break;
		default:
			break;
	}
}

void asm_get_r8_register(char (*register_text)[5], size_t size)
{
	memset(register_text, 0, 5);

	switch (size)
	{
		case 1:
			memcpy(register_text, "%r8b\0", 4);
			break;
		case 2:
			memcpy(register_text, "%r8w\0", 4);
			break;
		case 4:
			memcpy(register_text, "%r8d\0", 5);
			break;
		case 8:
			memcpy(register_text, "%r8\0", 5);
			break;
		default:
			break;
	}
}

void asm_get_r9_register(char (*register_text)[5], size_t size)
{
	memset(register_text, 0, 5);

	switch (size)
	{
		case 1:
			memcpy(register_text, "%r9b\0", 4);
			break;
		case 2:
			memcpy(register_text, "%r9w\0", 4);
			break;
		case 4:
			memcpy(register_text, "%r9d\0", 5);
			break;
		case 8:
			memcpy(register_text, "%r9\0", 5);
			break;
		default:
			break;
	}
}
