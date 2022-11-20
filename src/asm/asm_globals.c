#include "asm_globals.h"
#include "variables.h"
#include "global_scope.h"

bool asm_write_global_variables(
	FILE* asm_file,
	const struct global_scope_t* global_scope)
{
	for (size_t i = 0; i < global_scope->n_variables; ++i)
	{
		switch (global_scope->variables[i].type)
		{
			case STRING:
			{
				// strname: .asciz "value of string"
				const struct variable_t* variable = &global_scope->variables[i];
				fprintf(asm_file, "\t%s: .asciz %s\n",
					variable->name,
					variable->value);
				break;
			}
		}
	}

	return true;
}
