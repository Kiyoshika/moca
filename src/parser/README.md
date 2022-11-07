# MOCA Parser
The parser is one of the complicated pieces of writing a language and I feel it warrants its own readme.

# Parser Structure
This is an array-based "on the fly" parser, meaning we don't build a syntax tree or any specific data structure. We iterate through the token array using an iterator and modify that iterator to traverse different sections of the token array if necessary. We keep track of a "token buffer" which contains contextual information about what we're currently parsing as usually we won't know what to do until we have enough tokens (e.g., differentiating between defining a variable or defining a function).

There are a couple of helper structures to keep track of as we parse.
* `global_scope_t` - This keeps track of what `function_t`s and `variable_t`s are defined in the program (all definitions/declarations are added to global scope). Variables defined outside of functions are also added here (and would be defined in the `.data` section within assembly)
* `function_t` - This represents a function. Each function has a `stack_t` which holds `variable_t`s for any local variables defined inside. It also contains `parameters_t` specifically for holding any function parameters.

# Parser Components
## [Base Parser](parser.c)
This is the main driver behind the parser, it handles the initial setup and begins the iteration over the token array and delegates which subcomponents (subparser) to switch to depending on contextual information from the token buffer.
## [Definition Parser](parser_definition.c)
This is a dedicated parser for handling definitions. When we encounter a `DATATYPE` token (e.g., `int32`) we begin the definition parser. This will check if we're defining a variable or defining a function. If we are defining a variable, a `variable_t` will be created and added to the current function's stack; if we're not inside of a function it will be added to global scope. If we are defining a function, a `function_t` will be created and will be added to the global scope.
