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

add_subdirectory(samples)
