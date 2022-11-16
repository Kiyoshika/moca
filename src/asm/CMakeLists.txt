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
