# copy necessary files to build folder
configure_file(buffer.moca buffer.moca COPYONLY)
configure_file(mainfile.moca mainfile.moca COPYONLY)

# READ SOURCE BUFFER
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

# CREATE TOKEN
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

# CREATE TOKEN ARRAY
add_executable(create_token_array
	create_token_array.c
)
target_include_directories(create_token_array
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(create_token_array
	mocatokenarray
)

# CREATE TOKENIZER
add_executable(create_tokenizer
	create_tokenizer.c
)
target_include_directories(create_tokenizer
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(create_tokenizer
	mocasrcbuffer
	mocatokenizer
)

# CREATE LEXER
add_executable(create_lexer
	create_lexer.c
)
target_include_directories(create_lexer
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(create_lexer
	mocasrcbuffer
	mocatokenizer
	mocalexer
)
