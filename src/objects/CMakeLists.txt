# VARIABLES
add_library(mocavariable
	variables.c
)
target_include_directories(mocavariable
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/objects
)
target_link_libraries(mocavariable
	mocaerr
)

# PARAMETERS
add_library(mocaparameter
	parameters.c
)
target_include_directories(mocaparameter
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/objects
)
target_link_libraries(mocaparameter
	mocavariable
)

# FUNCTIONS
add_library(mocafunction
	functions.c
)
target_include_directories(mocafunction
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/objects
)
target_link_libraries(mocafunction
	mocavariable
	mocaparameter
)

# GLOBAL SCOPE
add_library(mocaglobalscope
	global_scope.c
)
target_include_directories(mocaglobalscope
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/objects
)
target_link_libraries(
	mocavariable
	mocafunction
)
