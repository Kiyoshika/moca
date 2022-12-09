#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "token.h"

#define VARIABLE_NAME_LEN 51

// forward declaration
struct err_msg_t;

struct variable_t
{
	char* value;
	size_t value_bytes_size;

	enum token_type_e type; // INT8, INT32, etc.

	char name[VARIABLE_NAME_LEN];

	size_t bytes_size; // 1, 2, 4, 8

	bool is_initialized;
	bool is_signed; // UINT8 vs INT8, etc.
};

// pass [variable] by address and initialize its members.
void variable_create(
		struct variable_t* variable);

// attempt to set a variable type.
// returns true on success, returns false otherwise and writes to [err].
bool variable_set_type(
		struct variable_t* variable,
		enum token_type_e type,
		struct err_msg_t* err);

// attempt to set a variable name (maximum of 50 chars).
// returns true on success, returns false otherwise and writes to [err].
bool variable_set_name(
		struct variable_t* variable,
		const char* name,
		struct err_msg_t* err);

// change whether or not the variable is initialized.
void variable_set_initialized(
		struct variable_t* variable,
		bool is_initialized);

// change whether or not the varialbe is signed
void variable_set_signed(
		struct variable_t* variable,
		bool is_signed);

// set value to variable.
bool variable_set_value(
		struct variable_t* variable,
		const char* value,
		struct err_msg_t* err);

// deallocate memory from variable->value (if set)
void variable_free(
		struct variable_t* variable);

#endif
