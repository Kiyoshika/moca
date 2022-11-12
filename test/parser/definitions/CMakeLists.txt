configure_file(variable_declaration.moca variable_declaration.moca COPYONLY)
configure_file(variable_definition.moca variable_definition.moca COPYONLY)
configure_file(missing_end_statement.moca missing_end_statement.moca COPYONLY)
configure_file(function_declaration.moca function_declaration.moca COPYONLY)
configure_file(function_definition.moca function_definition.moca COPYONLY)

# VARIABLE DECLARATION
add_executable(variable_declaration
	variable_declaration.c
)
target_include_directories(variable_declaration
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/parser
)
target_link_libraries(variable_declaration
	mocasrcbuffer
	mocatoken
	mocatokenarray
	mocatokenizer
	mocalexer
	mocaparser
	mocaerr
)
add_test(NAME parser_variable_declaration COMMAND variable_declaration)

# VARIABLE DEFINITION
add_executable(variable_definition
	variable_definition.c
)
target_include_directories(variable_definition
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/parser
)
target_link_libraries(variable_definition
	mocasrcbuffer
	mocatoken
	mocatokenarray
	mocatokenizer
	mocalexer
	mocaparser
	mocaerr
)
add_test(NAME parser_variable_definition COMMAND variable_definition)

# MISSING END STATEMENT
add_executable(missing_end_statement
	missing_end_statement.c
)
target_include_directories(missing_end_statement
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/parser
)
target_link_libraries(missing_end_statement
	mocasrcbuffer
	mocatoken
	mocatokenarray
	mocatokenizer
	mocalexer
	mocaparser
	mocaerr
)
add_test(NAME parser_missing_end_statement COMMAND missing_end_statement)

# FUNCTION DECLARATION
add_executable(function_declaration
	function_declaration.c
)
target_include_directories(function_declaration
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/parser
)
target_link_libraries(function_declaration
	mocasrcbuffer
	mocatoken
	mocatokenarray
	mocatokenizer
	mocalexer
	mocaparser
	mocaerr
)
add_test(NAME parser_function_declaration COMMAND function_declaration)

# FUNCTION DEFINITION
add_executable(function_definition
	function_definition.c
)
target_include_directories(function_definition
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/parser
)
target_link_libraries(function_definition
	mocasrcbuffer
	mocatoken
	mocatokenarray
	mocatokenizer
	mocalexer
	mocaparser
	mocaerr
)
add_test(NAME parser_function_definition COMMAND function_definition)
