# TYPES
add_executable(variable_types
	variable_types.c
)
target_include_directories(variable_types
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/objects
)
target_link_libraries(variable_types
	mocavariable
	mocatoken
	mocaerr
)
add_test(NAME variable_types COMMAND variable_types)
