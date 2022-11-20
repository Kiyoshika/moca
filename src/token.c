#include "token.h"

char token_list[N_TOKENS][MAX_TOKEN_LIST_LEN] = {
	" ",
	"!=",
	"\"",
	"%",
	"&&",
	"(",
	")",
	"*",
	"+",
	"++",
	"+=",
	",",
	"-",
	"--",
	"-=",
	"/",
	";",
	"<",
	"<=",
	"=",
	"==",
	">",
	">=",
	"int16",
	"int32",
	"int64",
	"int8",
	"string",
	"{",
	"||",
	"}"	
};

void tkn_create(
	struct token_t* token,
	const char* token_text,
	const size_t line_num,
	const size_t char_pos)
{
	// ensure text doesn't surpass 60 chars
	size_t text_len = strlen(token_text);
	text_len = text_len > 60 ? 60 : text_len;
	
	token->text_len = text_len;
	token->line_num = line_num;
	token->char_pos = char_pos;
	memcpy(&token->text[0], token_text, text_len);
	token->text[text_len] = '\0';

	token->type = TYPE_UNASSIGNED;
	token->category = CATEGORY_UNASSIGNED;
}

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
