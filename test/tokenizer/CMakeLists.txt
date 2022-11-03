configure_file(assignment.moca assignment.moca COPYONLY)
configure_file(ifstatement.moca ifstatement.moca COPYONLY)
configure_file(functions.moca functions.moca COPYONLY)
configure_file(singleoperators.moca singleoperators.moca COPYONLY)
configure_file(doubleoperators.moca doubleoperators.moca COPYONLY)
configure_file(forloops.moca forloops.moca COPYONLY)
configure_file(logicals.moca logicals.moca COPYONLY)

# ASSIGNMENT
add_executable(assignment assignment.c)
target_include_directories(assignment
	PUBLIC
	${moca_SOURCE_DIR}/include)
target_link_libraries(assignment
	mocasrcbuffer
	mocatokenarray
	mocatokenizer
)
add_test(NAME tokenizer_assignment COMMAND assignment)

# IF STATEMENTS
add_executable(ifstatement ifstatement.c)
target_include_directories(ifstatement
	PUBLIC
	${moca_SOURCE_DIR}/include)
target_link_libraries(ifstatement
	mocasrcbuffer
	mocatokenarray
	mocatokenizer
)
add_test(NAME tokenizer_ifstatement COMMAND ifstatement)

# FUNCTIONS
add_executable(functions functions.c)
target_include_directories(functions
	PUBLIC
	${moca_SOURCE_DIR}/include)
target_link_libraries(functions
	mocasrcbuffer
	mocatokenarray
	mocatokenizer
)

# SINGLE OPERATORS
add_executable(singleoperators singleoperators.c)
target_include_directories(singleoperators
	PUBLIC
	${moca_SOURCE_DIR}/include)
target_link_libraries(singleoperators
	mocasrcbuffer
	mocatokenarray
	mocatokenizer
)
add_test(NAME tokenizer_singleoperators COMMAND singleoperators)

# DOUBLE OPERATORS
add_executable(doubleoperators doubleoperators.c)
target_include_directories(doubleoperators
	PUBLIC
	${moca_SOURCE_DIR}/include)
target_link_libraries(doubleoperators
	mocasrcbuffer
	mocatokenarray
	mocatokenizer
)
add_test(NAME tokenizer_doubleoperators COMMAND doubleoperators)

# FOR LOOPS
add_executable(forloops forloops.c)
target_include_directories(forloops
	PUBLIC
	${moca_SOURCE_DIR}/include)
target_link_libraries(forloops
	mocasrcbuffer
	mocatokenarray
	mocatokenizer
)
add_test(NAME tokenizer_forloops COMMAND forloops)

# LOGICALS
add_executable(logicals logicals.c)
target_include_directories(logicals
	PUBLIC
	${moca_SOURCE_DIR}/include)
target_link_libraries(logicals
	mocasrcbuffer
	mocatokenarray
	mocatokenizer
)
add_test(NAME tokenizer_logicals COMMAND logicals)

