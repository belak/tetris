# Initial setup
cmake_minimum_required(VERSION 2.6)
project(tetris)

# Including any needed deps
include(FindPkgConfig)

# This is used for copying assets
function(copy_data target name destination)
	if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
		return()
	endif()

	file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/${name}/*)

	add_custom_target(${target} ALL DEPENDS ${files})

	foreach(file ${files})
		add_custom_command(
			OUTPUT "${destination}/${file}"
			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${file}"
			COMMAND "${CMAKE_COMMAND}" -E copy
				"${CMAKE_CURRENT_SOURCE_DIR}/${file}" "${destination}/${file}"
		)
	endforeach()
endfunction()

# Convenient options
option(CLANG "build application with clang" false)
option(DEBUG "Enable debugging output" true)

# Change the C and C++ compilers to clang if we want
if (CLANG)
	set(CMAKE_C_COMPILER "clang")
	set(CMAKE_CXX_COMPILER "clang++")
endif()

# Make it easy to enable debugging
if(DEBUG)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# Make sure we get all warnings and that we're using C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

pkg_check_modules(
	ALLEGRO REQUIRED
	allegro-5
	allegro_main-5
	allegro_ttf-5
	allegro_font-5
	allegro_primitives-5
	allegro_audio-5
	allegro_acodec-5
	allegro_color-5
	allegro_image-5
)

include_directories(
	${ALLEGRO_INCLUDE_DIRS}
)

file(GLOB_RECURSE assets assets/*)
file(GLOB_RECURSE source src/**.cpp)

set_source_files_properties(${source} PROPERTIES LANGUAGE "CXX")

add_executable(
	tetris

	${assets}
	${source}
)

target_link_libraries(
	tetris

	${ALLEGRO_LIBRARIES}
)

copy_data(assets assets .)
