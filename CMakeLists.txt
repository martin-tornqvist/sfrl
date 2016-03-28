# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------
project (sfrl)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/target)

# Exported compile commands can be used by Emacs Irony-Mode auto completion
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# Verification
# ------------------------------------------------------------------------------
# Splitting strings with backslashes is only supported in 3.x.x
# (There may be other dependencies too...)
cmake_minimum_required (VERSION 3.2.3)

# Require generating build files in a "build" directory (out of source builds)
if (NOT (${CMAKE_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR}/build))
  message (STATUS "CMAKE_BINARY_DIR   : ${CMAKE_BINARY_DIR}")
  message (STATUS "PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

  message (FATAL_ERROR
    " \
    \n Must run cmake from \"build\" directory! \
    \n \
    \n Hint: \
    \n $ mkdir build && cd build && cmake .. \
    \n \
    \n (note the trailing \"..\") \
    \n \
    \n You should probably delete CMakeCache.txt etc before trying again. \
    ")
endif ()

# ------------------------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------------------------
# Using c++14 standard
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)

set (CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} \
  -Wall \
  -Wextra \
  -pedantic \
  -fno-rtti \
  -fno-exceptions \
  ")

if ("$ENV{sfrl_mode}" STREQUAL "sdl_mode")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSDL_MODE")
else () # Not SDL mode
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCONSOLE_MODE")
endif ()

# ------------------------------------------------------------------------------
# Source files (*.cpp and *.hpp)
# ------------------------------------------------------------------------------
# Load external source file list
file (READ ${PROJECT_SOURCE_DIR}/source.txt SRC_FILES)
string (REPLACE "\n" ";" SRC_FILES ${SRC_FILES})

include_directories (
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/rl_utils/include
  )

# ------------------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------------------
add_executable (${PROJECT_NAME} ${SRC_FILES})

# ------------------------------------------------------------------------------
# Libraries
# ------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (WIN32)
	if ("$ENV{sfrl_mode}" STREQUAL "console_mode")
		# Use the pdcurses lib distributed with the project
		set (PDCURSES_DIR ${PROJECT_SOURCE_DIR}/libs/pdcurses)

		set (LIBS ${LIBS} ${PDCURSES_DIR}/pdcurses.lib)

		include_directories (${PDCURSES_DIR}/include)

		# Copy dll file(s) to target directory
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				"${PDCURSES_DIR}/bin"
				${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

	else () # SDL mode

		# Use the SDL2 lib distributed with the project

		set (SDL2_DIR
			${PROJECT_SOURCE_DIR}/libs/SDL2-2.0.3-i686-w64-mingw32)

		set (CMAKE_PREFIX_PATH ${SDL2_DIR})

		find_library (SDL2_LIBRARY       NAMES SDL2       REQUIRED)
		find_library (SDL2main_LIBRARY   NAMES SDL2main   REQUIRED)
		find_library (SDL2_image_LIBRARY NAMES SDL2_image REQUIRED)
		find_library (SDL2_ttf_LIBRARY   NAMES SDL2_ttf   REQUIRED)

		set (LIBS ${LIBS} ${SDL2_LIBRARY})
		set (LIBS ${LIBS} ${SDL2main_LIBRARY})
		set (LIBS ${LIBS} ${SDL2_image_LIBRARY})
		set (LIBS ${LIBS} ${SDL2_ttf_LIBRARY})

		find_path (SDL2_INCLUDE_DIR SDL.h)

		include_directories (${SDL2_INCLUDE_DIR})

		# Copy dll file(s) to target directory
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				"${SDL2_DIR}/bin"
				${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	endif()

else() # Not Windows platform

	if ("$ENV{sfrl_mode}" STREQUAL "console_mode")
		# Find ncurses lib
		set (CURSES_NEED_NCURSES TRUE)

		find_package (Curses REQUIRED)

		set (LIBS ${LIBS} ${CURSES_LIBRARIES})

		include_directories (${CURSES_INCLUDE_DIR})

	else () # Not console mode

		# Find SDL2 lib
		find_package (SDL2 REQUIRED)

		set (LIBS ${LIBS} ${SDL2_LIBRARIES})

		include_directories (${SDL2_INCLUDE_DIRS})
	endif()
endif()

# Lua
# set (LUA_DIR
# 	${PROJECT_SOURCE_DIR}/libs/lua-5.3.2_Win32_dllw4_lib)

# set (CMAKE_PREFIX_PATH ${LUA_DIR})

# find_library (LUA_LIBRARY NAMES lua53 REQUIRED)

# set (LIBS ${LIBS} ${LUA_LIBRARY})

# find_path (LUA_INCLUDE_DIR lua.h)

# include_directories (${LUA_INCLUDE_DIR})

# Copy dll file(s) to target directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${LUA_DIR}/bin"
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


target_link_libraries (${PROJECT_NAME} ${LIBS})

# ------------------------------------------------------------------------------
# Copy resources to target directory
# ------------------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/resources"
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
