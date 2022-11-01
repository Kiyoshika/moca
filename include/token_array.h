#ifndef TOKEN_ARRAY_H
#define TOKEN_ARRAY_H

#include "token.h"
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

struct token_array_t
{
	struct token_t* token;
	size_t length; // current number of tokens in array
	size_t capacity; // total POSSIBLE number of tokens
};

// Allocate [capacity] tokens for a token_array (passed by address).
// [length] will initially be set to 0.
// Returns true on success, false otherwise.
bool tkn_array_create(
	struct token_array_t* token_array,
	const size_t capacity);

// Deallocate memory held by [token_array].
// Resets length & capacity back to 0.
void tkn_array_free(
	struct token_array_t* token_array);

// Push a new [token] into the array. If length hits capacity, array is doubled in size.
// The [token]'s contents will be COPIED into the array rather than transferring ownership.
// Returns true on success, false otherwise.
bool tkn_array_push(
	struct token_array_t* token_array,
	const struct token_t* token);

// Resets array to 0 but doesn't deallocate any memory
void tkn_array_clear(
	struct token_array_t* token_array);


#endif
