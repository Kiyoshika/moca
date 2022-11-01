# SOURCE BUFFER
add_library(mocasrcbuffer
	source_buffer.c
)
target_include_directories(mocasrcbuffer
	PUBLIC
	${moca_SOURCE_DIR}/include
)

# TOKEN
add_library(mocatoken
	token.c
)
target_include_directories(mocatoken
	PUBLIC
	${moca_SOURCE_DIR}/include
)

# TOKEN ARRAY
add_library(mocatokenarray
	token_array.c
)
target_include_directories(mocatokenarray
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(mocatokenarray
	mocatoken
)

# TOKENIZER
add_library(mocatokenizer
	tokenizer.c
)
target_include_directories(mocatokenizer
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(mocatokenizer
	mocatokenarray
)

# LEXER
add_library(mocalexer
	lexer.c
)
target_include_directories(mocalexer
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(mocalexer
	mocatokenarray
)

add_subdirectory(samples)
