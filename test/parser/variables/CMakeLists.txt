configure_file(create_global_variables.moca create_global_variables.moca COPYONLY)
configure_file(strings.moca strings.moca COPYONLY)

# CREATE GLOBAL VARAIBLES
add_executable(create_global_variables
	create_global_variables.c
)
target_include_directories(create_global_variables
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/parser
)
target_link_libraries(create_global_variables
	mocasrcbuffer
	mocatoken
	mocatokenarray
	mocatokenizer
	mocalexer
	mocaparser
	mocavariable
	mocaglobalscope
	mocaerr
)
add_test(NAME parser_create_global_variables COMMAND create_global_variables)

# STRINGS
add_executable(strings
	strings.c
)
target_include_directories(strings
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/parser
)
target_link_libraries(strings
	mocasrcbuffer
	mocatoken
	mocatokenarray
	mocatokenizer
	mocalexer
	mocaparser
	mocavariable
	mocaglobalscope
	mocaerr
)
add_test(NAME parser_create_strings COMMAND strings)
