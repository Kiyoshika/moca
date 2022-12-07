# TODO LIST
(Not necessarily in order)

## BUGS:
* Providing non-existant moca file to comiler frontend causes it to crash
* No error is thrown when calling a function that is not initialised
* Don't subtract stack space when allocating strings, this is wasting a lot of memory (all strings are going into data section, no need to subtract stack space)
* Even when compiler fails, it generates partial assembly - only want to write the assembly if compilation succeeds

## LOGS:
* Add logging for each major step (tokenizer, parser, writing function instruction, etc.) to make things a lot easier to debug

## CLEANUP
* `_parse_variable` function inside `parser_create_variable.c` could use some cleanup, it's pretty messy
* All the string formatting in `asm_function.c` could use some cleanup. Currently a bunch of `%s` chained together, but could use more "clever"/cleaner formatting

# TESTS:
* Add return values back into `function_definition.moca` - after some changes to the parser, having return values will break since they are not implemented yet
* Update tests for parser_definition to check the correct `definition_type` enum (VARIABLE or FUNCTION)
* Write tests for `variable_t`, `function_t`, `parameter_t` and `global_scope_t`
* Add some more variable types and strings to the global variable tests
* Write tests for `function_write_instruction` and their translations into assembly

## CORE:
* Subtract stack space inside function (again)
	* Originally I had this, but it seemed to work without it...although to be extra safe I think I will subtract the stack space for all local variables
* Add global variables to data section (if initialized) or bss section (if uninitialized)
	* This currently works for strings, but not integer types
* Implement return values (`asm_functions.c`)
* Add line and char position information in `variable_t`, `function_t` and `parameter_t` which is used when encountering errors in `asm_functions.c`
* When fetching variable stack position, account for any parameters that don't fit in the first 6 registers (`asm_functions.c`)
* Check if variable name already exists (either parameter or local stack) when creating new variable inside function
* Add support for expressions during assignment/defintions (e.g., `int32 x = 3 + y - 1 * 5 / z`) - will need to translate these in a special way
* Create source file for all instructions (and their correct suffixes) called `asm_instructions.c` - this is very similar to `asm_registers.c` which contain all the registers used.
* Handle non-string global variables in `ADD_ARG` instruction inside `asm_functions.c`
* Push 7th+ parameters onto stack in `ADD_ARG` instruction inside `asm_functions.c`
* Change the main function logic to move the return value into rdi from rax
* Add support for returning string literals (currently only supports variable and numeric literal)

## NICE TO HAVE:
* Hardcode the SPACE token inside the `token_is_valid` function in `parse_definition.c` so we don't have to add it to the list of expected tokens each time.
* Improve `err_write` to not have to provide the line and char pos, we can set them in the parsers. Would be more convenient to just write the message as in some cases we don't have line/char information (e.g., when writing tests)
* Display +/- 5 lines of the original source code in `err_print`
* Implement `err_writef` for printing formatted error messages

