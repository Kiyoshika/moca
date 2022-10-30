# copy necessary files to build folder
configure_file(buffer.moca buffer.moca COPYONLY)

add_executable(read_source_buffer
	read_source_buffer.c
)
target_include_directories(read_source_buffer
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(read_source_buffer
	mocasrcbuffer
)

add_executable(create_token
	create_token.c
)
target_include_directories(create_token
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(create_token
	mocatoken
)
