# TODO LIST
(Not necessarily in order)

1. Add tests for DATATYPE category in lexer
2. Update tests for parser_definition to check the correct `definition_type` enum (VARIABLE or FUNCTION)
3. Add functions to global scope (parser.c)
4. Add variables to local function scope (depends on 3) (parser.c)
5. Implement basic instruction sets to translate parser structures to assembly
6. Display +/- 5 lines of the original source code in `err_print`
7. Write tests for `variable_t`, `function_t`, `parameter_t` and `global_scope_t`
8. Implement `err_printf` for printing formatted error messages
9. Fix memory leak in main functions that aren't freeing global scope.
10. Create actual COMMA token type and replace the expected token TEXT after TEXT in `parse_definition.c` with expected token COMMA after TEXT.
