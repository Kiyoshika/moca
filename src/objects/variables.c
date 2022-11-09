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

	variable->is_signed = false;

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
		};
		case INT16:
		{
			variable->type = INT16;
			variable->bytes_size = 2;
			return true;
		};
		case INT32:
		{
			variable->type = INT32;
			variable->bytes_size = 4;
			return true;
		};
		case INT64:
		{
			variable->type = INT64;
			variable->bytes_size = 8;
			return true;
		};
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

static bool failed_to_parse(
		struct variable_t* variable,
		struct err_msg_t* err)
{
	variable_free(variable);
	err_write(err, "Couldn't properly parse variable's value for the specified type.", 0, 0);

	return false;
}

bool variable_set_value(
		struct variable_t* variable,
		char* valuestr,
		bool is_negative,
		struct err_msg_t* err)
{
	if (!variable->value)
		variable->value = calloc(1, variable->bytes_size);

	char* endptr = NULL;

	switch (variable->type)
	{
		case INT8:
		{
			int8_t value = strtol(valuestr, &endptr, 10);
			if (strlen(endptr) > 0)
				return failed_to_parse(variable, err);
			value = is_negative ? -1 * value : value;
			memcpy(variable->value, &value, variable->bytes_size);
			break;
		}

		case INT16:
		{
			int16_t value = strtol(valuestr, &endptr, 10);
			if (strlen(endptr) > 0)
				return failed_to_parse(variable, err);
			value = is_negative ? -1 * value : value;
			memcpy(variable->value, &value, variable->bytes_size);
			break;
		}

		case INT32:
		{
			int32_t value = strtol(valuestr, &endptr, 10);
			if (strlen(endptr) > 0)
				return failed_to_parse(variable, err);
			value = is_negative ? -1 * value : value;
			memcpy(variable->value, &value, variable->bytes_size);
			break;
		}

		case INT64:
		{
			int64_t value = strtoll(valuestr, &endptr, 10);
			if (strlen(endptr) > 0)
				return failed_to_parse(variable, err);
			value = is_negative ? -1 * value : value;
			memcpy(variable->value, &value, variable->bytes_size);
			break;
		}

		default:
		{
			err_write(err, "Couldn't parse variable value for unknown type.", 0, 0);
			variable_free(variable);
			return false;
		}
	}
	
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
