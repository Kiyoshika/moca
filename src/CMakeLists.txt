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

# ERROR MESSAGE
add_library(mocaerr
	err_msg.c
)
target_include_directories(mocaerr
	PUBLIC
	${moca_SOURCE_DIR}/include
)


add_subdirectory(parser)
add_subdirectory(objects)

# UTIL
add_library(mocautil
	util.c
)
target_include_directories(mocautil
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/objects
)
target_link_libraries(mocautil
	mocafunction
	mocaparameter
	mocavariable
)

add_subdirectory(asm)
add_subdirectory(frontend)
