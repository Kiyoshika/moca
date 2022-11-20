# ASM FILES
add_library(moca-asmfile
	asm_file.c
)
target_include_directories(moca-asmfile
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/asm
	${moca_SOURCE_DIR}/include/objects
)

# ASM FUNCTIONS
add_library(moca-asmfunction
	asm_functions.c
)
target_include_directories(moca-asmfunction
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/asm
	${moca_SOURCE_DIR}/include/objects
)
target_link_libraries(moca-asmfunction
	moca-asmsection
)

# ASM SECTIONS
add_library(moca-asmsection
	asm_sections.c
)
target_include_directories(moca-asmsection
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/asm
	${moca_SOURCE_DIR}/include/objects
)

# ASM GLOBALS
add_library(moca-asmglobal
	asm_globals.c
)
target_include_directories(moca-asmglobal
	PUBLIC
	${moca_SOURCE_DIR}/include
	${moca_SOURCE_DIR}/include/asm
	${moca_SOURCE_DIR}/include/objects
)
target_link_libraries(moca-asmglobal
	mocavariable
)
