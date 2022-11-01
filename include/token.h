#ifndef TOKEN_H
#define TOKEN_H

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#define N_TOKENS 27
#define MAX_TOKEN_LIST_LEN 10

// ENSURE THIS IS SORTED IN ASCII ORDER ASCENDING
extern char token_list[N_TOKENS][MAX_TOKEN_LIST_LEN];

// ENSURE THIS HAS EXACT SAME ORDER AS [token_list]
enum token_type_e
{
	NOT_EQUAL,			// !=
	MODULO, 			// %
	LOGICAL_AND,		// &&
	OPEN_PAREN, 		// (
	CLOSE_PAREN, 		// )
	MULTIPLICATION, 	// *
	ADDITION, 			// +
	INCREMENT,			// ++
	PLUS_EQUALS, 		// +=
	SUBTRACTION, 		// -
	DECREMENT,			// --
	MINUS_EQUALS,		// -=
	DIVISION, 			// /
	END_STATEMENT,		// ;
	LESS_THAN,			// <
	LESS_THAN_EQ,		// <=
	ASSIGNMENT, 		// =
	EQUAL,				// ==
	GREATER_THAN,		// >
	GREATER_THAN_EQ,	// >=
	INT16,
	INT32,
	INT64,
	INT8,
	OPEN_BRACE, 		// {
	LOGICAL_OR,			// ||
	CLOSE_BRACE, 		// }

	TEXT = -1,
	NUMBER = -2,
	TYPE_UNASSIGNED = -3
};

enum token_category_e
{
	DATATYPE, // int32, int64, double, etc.
	OPERATOR, // +, -, /, *, %, =
	LOGICAL, // >, <, !=, etc.
	CATEGORY_UNASSIGNED = -1,
	NONE = -2
};

struct token_t
{
	char text[61]; 		// text content of the token
	size_t text_len;	// length of the text of the token
	size_t line_num; 	// line number token appears on; ranges from [1-inf]
	size_t char_pos; 	// character position on a given line number (line_num) [0-60]
	enum token_type_e type; // primary type that lexer assigns to a given token
	enum token_category_e category; // higher level token type category that lexer assigns to a given token
};

// Create a new token with [token_text] on a given [line_num] at position [char_pos]
// with respect to the current [line_num].
// Returns true if token was created successfully, false otherwise.
void tkn_create(
	struct token_t* token,
	const char* token_text,
	const size_t line_num,
	const size_t char_pos);

#endif
