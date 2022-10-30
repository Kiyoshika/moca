add_library(mocasrcbuffer
	source_buffer.c
)
target_include_directories(mocasrcbuffer
	PUBLIC
	${moca_SOURCE_DIR}/include
)

add_library(mocatoken
	token.c
)
target_include_directories(mocatoken
	PUBLIC
	${moca_SOURCE_DIR}/include
)

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

add_subdirectory(samples)
