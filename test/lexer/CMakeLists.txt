configure_file(logicals.moca logicals.moca COPYONLY)
configure_file(operators.moca operators.moca COPYONLY)
configure_file(textandnumeric.moca textandnumeric.moca COPYONLY)
configure_file(symbols.moca symbols.moca COPYONLY)
configure_file(datatypes.moca datatypes.moca COPYONLY)

# LOGICALS
add_executable(lexer_logicals lexer_logicals.c)
target_include_directories(lexer_logicals
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(lexer_logicals
	mocasrcbuffer
	mocatoken
	mocatokenizer
	mocalexer
)
add_test(NAME lexer_logicals COMMAND lexer_logicals)

# OPERATORS
add_executable(lexer_operators lexer_operators.c)
target_include_directories(lexer_operators
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(lexer_operators
	mocasrcbuffer
	mocatoken
	mocatokenizer
	mocalexer
)
add_test(NAME lexer_operators COMMAND lexer_operators)

# TEXT & NUMERIC
add_executable(lexer_textandnumeric lexer_textandnumeric.c)
target_include_directories(lexer_textandnumeric
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(lexer_textandnumeric
	mocasrcbuffer
	mocatoken
	mocatokenizer
	mocalexer
)
add_test(NAME lexer_textandnumeric COMMAND lexer_textandnumeric)

# SYMBOLS
add_executable(lexer_symbols lexer_symbols.c)
target_include_directories(lexer_symbols
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(lexer_symbols
	mocasrcbuffer
	mocatoken
	mocatokenizer
	mocalexer
)
add_test(NAME lexer_symbols COMMAND lexer_symbols)

# DATA TYPES
add_executable(lexer_datatypes lexer_datatypes.c)
target_include_directories(lexer_datatypes
	PUBLIC
	${moca_SOURCE_DIR}/include
)
target_link_libraries(lexer_datatypes
	mocasrcbuffer
	mocatoken
	mocatokenizer
	mocalexer
)
add_test(NAME lexer_datatypes COMMAND lexer_datatypes)

