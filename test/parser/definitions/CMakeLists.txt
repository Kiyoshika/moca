configure_file(too_many_datatype_tokens.moca too_many_datatype_tokens.moca COPYONLY)

# TOO MANY DATATYPE TOKENS
add_executable(too_many_datatype_tokens
	parser_too_many_datatype_tokens.c
)
target_include_directories(too_many_datatype_tokens
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/parser
)
target_link_libraries(too_many_datatype_tokens
	mocasrcbuffer
	mocatoken
	mocatokenarray
	mocatokenizer
	mocalexer
	mocaparser
	mocaerr
)
add_test(NAME parser_too_many_datatype_tokens COMMAND too_many_datatype_tokens)
