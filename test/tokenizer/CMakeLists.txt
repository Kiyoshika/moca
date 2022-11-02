configure_file(assignment1.moca assignment1.moca COPYONLY)
configure_file(assignment2.moca assignment2.moca COPYONLY)


add_executable(assignment assignment.c)
target_include_directories(assignment
	PUBLIC
	${moca_SOURCE_DIR}/include)
target_link_libraries(assignment
	mocasrcbuffer
	mocatokenarray
	mocatokenizer
)
add_test(NAME tokenizer_assignment COMMAND assignment)
