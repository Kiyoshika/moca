#include "variables.h"
#include "err_msg.h"

void variable_create(
		struct variable_t* variable)
{
	variable->value = NULL;
	variable->value_bytes_size = 0;

	variable->type = NONE;

	memset(variable->name, 0, VARIABLE_NAME_LEN);

	variable->bytes_size = 0;

	variable->is_initialized = false;

	// signed variables are probably more common, so we'll make
	// that the default
	variable->is_signed = true;

}

bool variable_set_type(
		struct variable_t* variable,
		enum token_type_e type,
		struct err_msg_t* err)
{
	switch (type)
	{
		case INT8:
		{
			variable->type = INT8;
			variable->bytes_size = 1;
			return true;
		}
		case INT16:
		{
			variable->type = INT16;
			variable->bytes_size = 2;
			return true;
		}
		case INT32:
		{
			variable->type = INT32;
			variable->bytes_size = 4;
			return true;
		}
		case INT64:
		{
			variable->type = INT64;
			variable->bytes_size = 8;
			return true;
		}
		case STRING:
		{
			variable->type = STRING;
			variable->bytes_size = 8; // we pass addresses of strings, which are 8 bytes
			return true;
		}
		default:
		{
			err_write(err, "Attempted to create variable with invalid type.", 0, 0);
			return false;
		};
	}

	return false;
}

bool variable_set_name(
		struct variable_t* variable,
		const char* name,
		struct err_msg_t* err)
{
	if (strlen(name) > 50)
	{
		err_write(err, "Variable name cannot exceed 50 characters.", 0, 0);
		return false;
	}

	memcpy(variable->name, name, strlen(name));
	variable->name[strlen(name)] = '\0';

	return true;
}

void variable_set_initialized(
		struct variable_t* variable,
		bool is_initialized)
{
	variable->is_initialized = is_initialized;
}

void variable_set_signed(
		struct variable_t* variable,
		bool is_signed)
{
	variable->is_signed = is_signed;
}

bool variable_set_value(
		struct variable_t* variable,
		const char* valuestr,
		struct err_msg_t* err)
{
	if (variable->value)
		free(variable->value);
	
	variable->value = strdup(valuestr);
	if (!variable->value)
	{
		err_write(err, "Couldn't allocate memory for variable value.", 0, 0);
		return false;
	}
	if (variable->type == STRING)
		variable->bytes_size = strlen(valuestr);
	
	return true;
}

void variable_free(
		struct variable_t* variable)
{
	if (variable->value)
	{
		free(variable->value);
		variable->value = NULL;
	}
}
