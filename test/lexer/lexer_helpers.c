char* type_to_text(enum token_type_e type)
{
	switch (type)
	{
		case NOT_EQUAL:
			return "!=";
		case MODULO:
			return "%";
		case LOGICAL_AND:
			return "&&";
		case OPEN_PAREN:
			return "(";
		case CLOSE_PAREN:
			return ")";
		case MULTIPLICATION:
			return "*";
		case ADDITION:
			return "+";
		case INCREMENT:
			return "++";
		case PLUS_EQUALS:
			return "+=";
		case SUBTRACTION:
			return "-";
		case DECREMENT:
			return "--";
		case MINUS_EQUALS:
			return "-=";
		case DIVISION:
			return "/";
		case END_STATEMENT:
			return ";";
		case LESS_THAN:
			return "<";
		case LESS_THAN_EQ:
			return "<=";
		case ASSIGNMENT:
			return "=";
		case EQUAL:
			return "==";
		case GREATER_THAN:
			return ">";
		case GREATER_THAN_EQ:
			return ">=";
		case INT16:
			return "int16";
		case INT32:
			return "int32";
		case INT64:
			return "int64";
		case INT8:
			return "int8";
		case OPEN_BRACE:
			return "{";
		case LOGICAL_OR:
			return "||";
		case CLOSE_BRACE:
			return "}";

		case TEXT:
			return "TEXT";
		case NUMBER:
			return "NUMBER";
		case TYPE_UNASSIGNED:
			return "UNASSIGNED";

		default:
			return NULL;
	}

	return NULL;
}

char* category_to_text(enum token_category_e category)
{
	switch (category)
	{
		case DATATYPE:
			return "DATATYPE";
		case OPERATOR:
			return "OPERATOR";
		case LOGICAL:
			return "LOGICAL";
		case CATEGORY_UNASSIGNED:
			return "UNASSIGNED";
		case NONE:
			return "NONE";

		default:
			return NULL;
	}

	return NULL;
}
