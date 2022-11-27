#include "function_prototype.h"
#include "token.h"
#include <stdio.h>

int main()
{
	// function with no args
	struct function_prototype_t fproto;
	func_proto_init(&fproto);
	func_proto_set_name(&fproto, "noargs");

	if (strcmp("noargs", fproto.name) != 0)
	{
		fprintf(stderr, "expected function name 'noargs' but got '%s'.\n", fproto.name);
		func_proto_free(&fproto);
		return -1;
	}

	if (fproto.n_parameters != 0)
	{
		fprintf(stderr, "expected 0 parameters but got %zu.\n", fproto.n_parameters);
		func_proto_free(&fproto);
		return -1;
	}

	func_proto_set_name(&fproto, "threeargs");
	if (strcmp("threeargs", fproto.name) != 0)
	{
		fprintf(stderr, "expected function name 'threeargs' but got '%s'.\n", fproto.name);
		func_proto_free(&fproto);
		return -1;
	}

	// reassign to have three args
	enum token_type_e set_args[3] = {
		STRING,
		INT32,
		INT8
	};
	func_proto_set_arg_types(&fproto, set_args, 3);

	if (fproto.n_parameters != 3)
	{
		fprintf(stderr, "expected 3 parameters but got %zu.\n", fproto.n_parameters);
		func_proto_free(&fproto);
		return -1;
	}

	if (fproto.parameter_types[0] != STRING)
	{
		fprintf(stderr, "expected first parameter to be of type STRING but got %s.\n", type_to_text(fproto.parameter_types[0]));
		func_proto_free(&fproto);
		return -1;
	}

	if (fproto.parameter_types[1] != INT32)
	{
		fprintf(stderr, "expected first parameter to be of type INT32 but got %s.\n", type_to_text(fproto.parameter_types[1]));
		func_proto_free(&fproto);
		return -1;
	}

	if (fproto.parameter_types[2] != INT8)
	{
		fprintf(stderr, "expected first parameter to be of type INT8 but got %s.\n", type_to_text(fproto.parameter_types[2]));
		func_proto_free(&fproto);
		return -1;
	}

	func_proto_free(&fproto);
	return 0;
}
