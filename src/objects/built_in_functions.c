#include "built_in_functions.h"
#include "global_scope.h"
#include "functions.h"
#include "variables.h"
#include "parameters.h"

char* built_in_functions[N_BUILT_IN_FUNCTIONS] = {
	"printf"
};

void built_in_functions_create(struct global_scope_t* global_scope)
{
	// printf - for now we will force only a string argument but later
	// will need to add arbitrary arguments
	// TODO:
}
