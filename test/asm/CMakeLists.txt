configure_file(sample.moca sample.moca COPYONLY)

add_executable(asmcreatefunctions
	asm_create_functions.c
)
target_include_directories(asmcreatefunctions
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/objects
	${moca_SOURCE_DIR}/include/asm
	${moca_SOURCE_DIR}/include/parser
)
target_link_libraries(asmcreatefunctions
	mocatoken
	mocaglobalscope
	mocasrcbuffer
	mocatokenarray
	mocatokenizer
	mocalexer
	mocaerr
	mocaparser
	mocafunction
	mocavariable
	moca-asmfile
	moca-asmfunction
)
