#include "variables.h"
#include "err_msg.h"
#include "token.h"
#include <stdio.h>

int validate_type(
		struct variable_t* variable,
		enum token_type_e type,
		size_t expected_bytes_size,
		struct err_msg_t* err)
{
	variable_set_type(variable, type, err);
	if (variable->bytes_size != expected_bytes_size)
	{
		fprintf(stderr, "Expected %zu bytes for '%s', got %zu\n", 
				expected_bytes_size,
				type_to_text(type),
				variable->bytes_size);
		return -1;

	}

	return 0;
}

int main()
{
	struct err_msg_t err;

	struct variable_t variable;
	variable_create(&variable);

	int testval = 0;

	testval = validate_type(&variable, INT8, 1, &err);
	if (testval == -1) return testval;

	testval = validate_type(&variable, INT16, 2, &err);
	if (testval == -1) return testval;

	testval = validate_type(&variable, INT32, 4, &err);
	if (testval == -1) return testval;

	testval = validate_type(&variable, INT64, 8, &err);
	if (testval == -1) return testval;

	return 0;

}
