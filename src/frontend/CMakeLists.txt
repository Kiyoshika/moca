add_executable(moca moca.c)
target_include_directories(moca
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_IDR}/include/parser
	${moca_SOURCE_DIR}/include/asm
	${moca_SOURCE_DIR}/include/objects
)
target_link_libraries(moca
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
	moca-asmsection
	moca-asmglobal
)
