#include "built_in_functions.h"
#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "parameters.h"
#include "err_msg.h"
#include "function_prototype.h"

char* built_in_functions[N_BUILT_IN_FUNCTIONS] = {
	"printf"
};

static bool _create_printf(
	struct global_scope_t* global_scope,
	struct err_msg_t* err)
{
	// initially printf will only take a STRING argument, but
	// will be parsed for different formats (%d, %s, etc.) which
	// can dynamically expand the parameter types
	struct function_prototype_t _printf;
	func_proto_init(&_printf);
	func_proto_set_name(&_printf, "printf");
	
	enum token_type_e printf_args[1] = { STRING };
	func_proto_set_arg_types(&_printf, printf_args, 1);

	if (!gscope_add_function_prototype(global_scope, &_printf, err))
	{
		func_proto_free(&_printf);
		return false;
	}

	return true;
}

bool built_in_functions_create(
	struct global_scope_t* global_scope,
	struct err_msg_t* err)
{
	if (!_create_printf(global_scope, err))
		return false;
	
	return true;
}
