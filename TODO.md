# TODO LIST
(Not necessarily in order)

* Add tests for DATATYPE category in lexer
* Update tests for parser_definition to check the correct `definition_type` enum (VARIABLE or FUNCTION)
* Add global variables to data section (if initialized) or bss section (if uninitialized)
* Add line and char position information in `variable_t`, `function_t` and `parameter_t` which is used when encountering errors in `asm_functions.c`
* Implement return values (`asm_functions.c`)
* When fetching variable stack position, account for any parameters that don't fit in the first 6 registers (`asm_functions.c`)
* Improve `err_write` to not have to provide the line and char pos, we can set them in the parsers. Would be more convenient to just write the message as in some cases we don't have line/char information (e.g., when writing tests)
* Check if variable name already exists (either parameter or local stack) when creating new variable inside function
* Display +/- 5 lines of the original source code in `err_print`
* Write tests for `variable_t`, `function_t`, `parameter_t` and `global_scope_t`
* Implement `err_writef` for printing formatted error messages
* Create actual COMMA token type and replace the expected token TEXT after TEXT in `parse_definition.c` with expected token COMMA after TEXT.
