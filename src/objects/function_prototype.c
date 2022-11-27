#include "function_prototype.h"
#include "token.h"

struct function_prototype_t _printf;

void func_proto_init(
	struct function_prototype_t* function_prototype)
{
	memset(function_prototype->name, 0, FUNC_PROTO_NAME_LEN);
	function_prototype->parameter_types = NULL;
	function_prototype->n_parameters = 0;
}

void func_proto_set_name(
	struct function_prototype_t* function_prototype,
	const char* name)
{
	size_t name_len = strlen(name);
	size_t max_len = (size_t)FUNC_PROTO_NAME_LEN - 1;
	name_len = name_len > max_len ? max_len : name_len;
	memset(function_prototype->name, 0, FUNC_PROTO_NAME_LEN); // clear name
	memcpy(function_prototype->name, name, name_len);
	function_prototype->name[name_len] = '\0';
}

bool func_proto_set_arg_types(
	struct function_prototype_t* function_prototype,
	const enum token_type_e* parameter_types,
	const size_t n_parameters)
{
	void* alloc = realloc(function_prototype->parameter_types, n_parameters * sizeof(enum token_type_e));
	if (!alloc)
		return false;
	function_prototype->parameter_types = alloc;
	function_prototype->n_parameters = n_parameters;

	for (size_t i = 0; i < n_parameters; ++i)
		function_prototype->parameter_types[i] = parameter_types[i];

	return true;
}

void func_proto_free(
	struct function_prototype_t* function_prototype)
{
	free(function_prototype->parameter_types);
	function_prototype->parameter_types = NULL;

	function_prototype->n_parameters = 0;
	memset(function_prototype->name, 0, FUNC_PROTO_NAME_LEN);
}
