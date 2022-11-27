# FUNCTION PROTOTYPES
add_executable(function_prototypes
	function_prototypes.c
)
target_include_directories(function_prototypes
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/objects
	${moca_SOURCE_DIR}/include/objects/functions
)
target_link_libraries(function_prototypes
	mocafunction
	mocatoken
)
add_test(NAME function_prototypes COMMAND function_prototypes)
