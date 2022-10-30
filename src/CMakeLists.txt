add_library(mocasrcbuffer
	source_buffer.c
)
target_include_directories(mocasrcbuffer
	PUBLIC
	${moca_SOURCE_DIR}/include
)

add_subdirectory(samples)
