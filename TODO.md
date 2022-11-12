# TODO LIST
(Not necessarily in order)

* Add tests for DATATYPE category in lexer
* Update tests for parser_definition to check the correct `definition_type` enum (VARIABLE or FUNCTION)
* Add variables to local stack (`parser_create_function.c`) - currently parameters are working and test is written
* Improve `err_write` to not have to provide the line and char pos, we can set them in the parsers. Would be more convenient to just write the message as in some cases we don't have line/char information (e.g., when writing tests)
* Check if variable name already exists (either parameter or local stack) when creating new variable inside function
* Implement basic instruction sets to translate parser structures to assembly
* Display +/- 5 lines of the original source code in `err_print`
* Write tests for `variable_t`, `function_t`, `parameter_t` and `global_scope_t`
* Implement `err_writef` for printing formatted error messages
* Create actual COMMA token type and replace the expected token TEXT after TEXT in `parse_definition.c` with expected token COMMA after TEXT.
