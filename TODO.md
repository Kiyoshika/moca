# TODO LIST
(Not necessarily in order)

## Developer Guide
* Definitely need to start documenting details related to development towards moca now that it's becoming a bit more stable. Describe how the parser works, the function instructions, translating those function instructions into assembly, etc.

## BUGS:
* Providing non-existant moca file to comiler frontend causes it to crash
* No error is thrown when calling a function that is not initialised
* Don't subtract stack space when allocating strings, this is wasting a lot of memory (all strings are going into data section, no need to subtract stack space)
* Even when compiler fails, it generates partial assembly - only want to write the assembly if compilation succeeds
* Due to the string renaming (when they're globally allocated) an "Unknown variable name" error is thrown when trying to reassign a value. To fix this, probably add another copy to the local function scope for strings (with the unconverted name)

## LOGS:
* Add logging for each major step (tokenizer, parser, writing function instruction, etc.) to make things a lot easier to debug

## CLEANUP
* `_parse_variable` function inside `parser_create_variable.c` could use some cleanup, it's pretty messy
* All the string formatting in `asm_function.c` could use some cleanup. Currently a bunch of `%s` chained together, but could use more "clever"/cleaner formatting
* Create a new `asm_tasks.c` that contain common functionalities such as moving to/from the stack, moving between registers, etc. which would help remove the hard-coded printf statements in things such as `asm_functions.c`
* Possibly create a `variable_info_t` struct that holds `index`, `stack_position`, `bytes_size` and `is_parameter` to make it convenient to pass around to utility functions, e.g., `util_find_variable_position`
* Move some of the major logic in `asm_functions.c` such as `_assign_variable` and `_return_function` etc. into their own dedicated (internal) source files. This will make `asm_functions.c` easier to read (can just directly include the internal functions via header)

# TESTS:
* Update tests for `parser_definition` to check the correct `definition_type` enum (VARIABLE or FUNCTION)
* Write tests for `variable_t`, `function_t`, `parameter_t` and `global_scope_t`
* Add some more variable types and strings to the global variable tests
* Write tests for `function_write_instruction` and their translations into assembly

## CORE:
* Add global variables to data section (if initialized) or bss section (if uninitialized)
	* This currently works for strings, but not integer types
* Add line and char position information in `variable_t`, `function_t` and `parameter_t` which is used when encountering errors in `asm_functions.c`
* When fetching variable stack position, account for any parameters that don't fit in the first 6 registers (`asm_functions.c`)
* Check if variable name already exists (either parameter or local stack) when creating new variable inside function
* Add support for expressions during assignment/defintions (e.g., `int32 x = 3 + y - 1 * 5 / z`) - will need to translate these in a special way
* Create source file for all instructions (and their correct suffixes) called `asm_instructions.c` - this is very similar to `asm_registers.c` which contain all the registers used.
* Handle non-string global variables in `ADD_ARG` instruction inside `asm_functions.c`
* Push 7th+ parameters onto stack in `ADD_ARG` instruction inside `asm_functions.c`
* Change the main function logic to move the return value into rdi from rax
* Add support for assigning variable from function assignment (e.g., `x = get_value();`)

## NICE TO HAVE:
* Add functions in utility for thinks like `util_get_token_type` and `util_get_token_size` etc. which take a token array and an index. These are very common operations performed especially in parser code.
* Hardcode the SPACE token inside the `token_is_valid` function in `parse_definition.c` so we don't have to add it to the list of expected tokens each time.
* Improve `err_write` to not have to provide the line and char pos, we can set them in the parsers. Would be more convenient to just write the message as in some cases we don't have line/char information (e.g., when writing tests)
* Display +/- 5 lines of the original source code in `err_print`
* Implement `err_writef` for printing formatted error messages

