cmake_minimum_required(VERSION 3.10)
project(moca C)

option(CMAKE_BUILD_TYPE "Release")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g -Wall") 		# cmake -DCMAKE_BUILD_TYPE=Debug ..
set(CMAKE_C_FLAGS_RELEASE "-O2 -Wall") 			# cmake -DCMAKE_BUILD_TYPE=Debug ..
if (${CMAKE_BUILD_TYPE} STREQUAL "DebugASAN")	# cmake -DCMAKE_BUILD_TYPE=DebugASAN ..
	set(CMAKE_C_FLAGS "-O0 -g -Wall -fsanitize=address")
endif()

add_subdirectory(src)
