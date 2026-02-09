cmake_minimum_required(VERSION 3.10)

project(main LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_link_options("-fuse-ld=bfd")
	add_compile_options(-Wall -Wextra -Wpedantic -Wold-style-cast)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_link_options("-fuse-ld=lld")
	add_compile_options(-Wall -Wextra -Wpedantic -Wold-style-cast)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_link_options("-fuse-ld=link")
	add_compile_options(/Wall /permissive- /fsanitize=address /utf-8)
endif()

# set main entry point
add_executable(${PROJECT_NAME} src/main.cpp)

# scan pkg/<module>/<header>
file(GLOB_RECURSE DEPENDENCIES_DIR LIST_DIRECTORIES
                                   true
                                   CONFIGURE_DEPENDS
	#───────────────────────────────────────────────#
	  "${CMAKE_SOURCE_DIR}/pkg/**/*.{h,hh,hpp,hxx}" #
	# "${CMAKE_SOURCE_DIR}/pkg/**/*.{c,cc,cpp,cxx}" #
	#───────────────────────────────────────────────#
)
list(REMOVE_DUPLICATES DEPENDENCIES_DIR)

# IDE specific (Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES
	VS_GLOBAL_RunCodeAnalysis             true
	VS_GLOBAL_EnableMicrosoftCodeAnalysis true
	VS_GLOBAL_EnableClangTidyCodeAnalysis true
	DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# resolve pkg/<module>/<header>
target_include_directories(${PROJECT_NAME} PRIVATE ${DEPENDENCIES_DIR}
                                           PRIVATE ${CMAKE_SOURCE_DIR}/src)
