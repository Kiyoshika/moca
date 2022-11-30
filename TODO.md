# TODO LIST
(Not necessarily in order)

# BUGS:
* Providing non-existant moca file to comiler frontend causes it to crash
* Function names break with underscores (similar to before with variable names)
* Passing a variable that holds an integer to `printf` in the first argument doesn't throw an error (when it should) like it does for passing numerical literals

# LOGS:
* Add logging for each major step (tokenizer, parser, writing function instruction, etc.) to make things a lot easier to debug

# CLEANUP
* `_parse_variable` function inside `parser_create_variable.c` could use some cleanup, it's pretty messy
* All the string formatting in `asm_function.c` could use some cleanup. Currently a bunch of `%s` chained together, but could use more "clever"/cleaner formatting

# TESTS:
* Add return values back into `function_definition.moca` - after some changes to the parser, having return values will break since they are not implemented yet
* Update tests for parser_definition to check the correct `definition_type` enum (VARIABLE or FUNCTION)
* Write tests for `variable_t`, `function_t`, `parameter_t` and `global_scope_t`
* Add some more variable types and strings to the global variable tests
* Write tests for `function_write_instruction` and their translations into assembly

# CORE:
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
* Put all of parameters into stack inside function, then allocate any additional local variables. This is incase any of the registers happen to be used as temp registers, we will always have access to them on the stack.

# NICE TO HAVE:
* Hardcode the SPACE token inside the `token_is_valid` function in `parse_definition.c` so we don't have to add it to the list of expected tokens each time.
* Improve `err_write` to not have to provide the line and char pos, we can set them in the parsers. Would be more convenient to just write the message as in some cases we don't have line/char information (e.g., when writing tests)
* Display +/- 5 lines of the original source code in `err_print`
* Implement `err_writef` for printing formatted error messages

