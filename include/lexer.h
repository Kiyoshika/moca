#ifndef LEXER_H
#define LEXER_H

// forward declaration
struct token_array_t;

// Pass [token_array] by address and assign each token a type (and possibly category)
// indicating whether it's a datatype, operator, bracket or some other token type.
void lexer_parse(
	struct token_array_t* token_array);

#endif
