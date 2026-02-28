cmake_minimum_required(VERSION 3.10)

project(x69 LANGUAGES C CXX)

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

# setup module
set(CMAKE_CURRENT_PROJECTDIR
  # sets project folder to "./src"
  ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(x69_string INTERFACE)

target_compile_features(x69_string
	PUBLIC INTERFACE
		# enforce project's standard
		cxx_std_${CMAKE_CXX_STANDARD}
)

target_include_directories(x69_string
	SYSTEM INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_PROJECTDIR}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# setup script
add_executable(tests tests/main.cpp)
add_executable(tools tools/main.cpp)

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

# IDE specifics (Visual Studio)
set_target_properties(tests tools PROPERTIES
	VS_GLOBAL_RunCodeAnalysis             true
	VS_GLOBAL_EnableMicrosoftCodeAnalysis true
	VS_GLOBAL_EnableClangTidyCodeAnalysis true
	DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# resolve pkg/<module>/<header>
target_link_libraries(tests PRIVATE x69_string)
target_include_directories(tests PRIVATE ${DEPENDENCIES_DIR})

target_link_libraries(tools PRIVATE x69_string)
target_include_directories(tools PRIVATE ${DEPENDENCIES_DIR})
