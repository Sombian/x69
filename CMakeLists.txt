cmake_minimum_required(VERSION 3.10)

set(CMAKE_POLICY_DEFAULT_CMP0000 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

project(x69 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERIFY_INTERFACE_HEADER_SETS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 0)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 0)

set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES 0)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 0)

add_compile_options("$<$<BUILD_INTERFACE:$<CXX_COMPILER_ID:MSVC>>:/utf-8;/Wall;/permissive-;/fsanitize=address>"
                    "$<$<BUILD_INTERFACE:$<CXX_COMPILER_ID:GNU>>:-g;-Wall;-Wextra;-Wpedantic;-Wcast-align;-Wshadow>"
                    "$<$<BUILD_INTERFACE:$<CXX_COMPILER_ID:Clang>>:-g;-Wall;-Wextra;-Wpedantic;-Wcast-align;-Wshadow>")

# add_link_options("$<$<BUILD_INTERFACE:$<CXX_COMPILER_ID:GNU>>:-fuse-ld=lld>"
#                  "$<$<BUILD_INTERFACE:$<CXX_COMPILER_ID:Clang>>:-fuse-ld=lld>")

file(GLOB GIT_MOD FOLLOW_SYMLINKS LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/pkg/*")
file(GLOB x69_MOD FOLLOW_SYMLINKS LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*")

#----------#
#  𝑓 -> ?  #
#----------#

add_library(CLANGD_FIX OBJECT EXCLUDE_FROM_ALL)

if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
	# fix; CLANGD compile_commands.json discovery
	file(COPY_FILE "${CMAKE_BINARY_DIR}/compile_commands.json"
	               "${CMAKE_SOURCE_DIR}/compile_commands.json"
	               # skip unnecessary file copies
	               ONLY_IF_DIFFERENT INPUT_MAY_BE_RECENT)
endif()

target_compile_options(CLANGD_FIX PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/Zs>
                                          $<$<CXX_COMPILER_ID:GNU>:-fsyntax-only>
                                          $<$<CXX_COMPILER_ID:Clang>:-fsyntax-only>)

function(x69_init_feat PKG)
	set(DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${PKG})

	add_library(${PKG} INTERFACE) # package

	target_include_directories(${PKG} INTERFACE ${DIR})

	file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "${DIR}/*.hpp"
	                                        "${DIR}/*.inl")

	foreach(_ ${SRC})
		set_source_files_properties(${_} PROPERTIES LANGUAGE CXX
			# fix: CLANGD fs discovery
			COMPILE_OPTIONS "-I${DIR}" COMPILE_DEFINITIONS "POISON")

		set_property(SOURCE ${_} APPEND PROPERTY COMPILE_OPTIONS
			# fix: deps.txt discovery
			"$<TARGET_PROPERTY:${PKG},INCLUDE_DIRECTORIES>"
			"$<TARGET_PROPERTY:${PKG},INTERFACE_INCLUDE_DIRECTORIES>")

		set_property(SOURCE ${_} APPEND PROPERTY COMPILE_OPTIONS
			# ...copies compile opts
			"$<TARGET_PROPERTY:${PKG},COMPILE_OPTIONS>"
			"$<TARGET_PROPERTY:${PKG},INTERFACE_COMPILE_OPTIONS>")

		set_property(SOURCE ${_} APPEND PROPERTY COMPILE_DEFINITIONS
			# ...copies compile defs
			"$<TARGET_PROPERTY:${PKG},COMPILE_DEFINITIONS>"
			"$<TARGET_PROPERTY:${PKG},INTERFACE_COMPILE_DEFINITIONS>")
	endforeach()
	# make CLANGD LSP aware of the header(s)
	target_sources(CLANGD_FIX PRIVATE ${SRC})
endfunction()

function(x69_init_test PKG)
	set(DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/${PKG})

	add_executable(test_${PKG} "${DIR}/test.cpp")
	target_link_libraries(test_${PKG} PRIVATE ${PKG})

	target_compile_options(test_${PKG} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/FI x69/${PKG}.hpp>
	                                           $<$<CXX_COMPILER_ID:GNU>:-include;x69/${PKG}.hpp>
	                                           $<$<CXX_COMPILER_ID:Clang>:-include;x69/${PKG}.hpp>)

	set_target_properties(test_${PKG} PROPERTIES VS_GLOBAL_EnableMicrosoftCodeAnalysis true
	                                             VS_GLOBAL_EnableClangTidyCodeAnalysis true
	                                  # uniform dev environment
	                                  DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

	if(MSVC)
		get_filename_component(_ "${CMAKE_CXX_COMPILER}" DIRECTORY)

		add_custom_command(TARGET test_${PKG} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different # [!] deferred file copy
		                                                 "${_}/clang_rt.asan_dynamic-x86_64.dll" $<TARGET_FILE_DIR:test_${PKG}>
		                                                 COMMENT "Copying MSVC::AddressSanitizer DLL") # [!] why must I manually...
	endif()
endfunction()

#----------#
#  (init)  #
#----------#

foreach(PATH ${GIT_MOD})
	if(IS_DIRECTORY ${PATH})
		  get_filename_component(NAME ${PATH} NAME)
		# get_filename_component(FOLD ${PATH} PATH)

		add_subdirectory(${PATH} SYSTEM)

		message(STATUS "use (${NAME})")
	endif()
endforeach()

foreach(PATH ${x69_MOD})
	if(IS_DIRECTORY ${PATH})
		  get_filename_component(NAME ${PATH} NAME)
		# get_filename_component(FOLD ${PATH} PATH)

		x69_init_feat(${NAME}) # expands
		x69_init_test(${NAME}) # expands

		message(STATUS "pkg {x69::${NAME}}")
	endif()
endforeach()

#----------#
# deps.txt #
#----------#

foreach(PATH ${x69_MOD})
	if(IS_DIRECTORY ${PATH})
		  get_filename_component(NAME ${PATH} NAME)
		# get_filename_component(FOLD ${PATH} PATH)

		set(FEAT_TXT "${CMAKE_CURRENT_SOURCE_DIR}/src/${NAME}/deps.txt")

		if(EXISTS ${FEAT_TXT})
			file(STRINGS ${FEAT_TXT} FEAT_DEPS)

			set_property(DIRECTORY APPEND PROPERTY
			CMAKE_CONFIGURE_DEPENDS "${FEAT_TXT}")

			foreach(DEP ${FEAT_DEPS})
				string(STRIP ${DEP} DEP)
				if(NOT DEP MATCHES "^#" AND NOT DEP STREQUAL "")
					target_link_libraries(${NAME} INTERFACE ${DEP})
				endif()
			endforeach()
		endif()

		set(TEST_TXT "${CMAKE_CURRENT_SOURCE_DIR}/test/${NAME}/deps.txt")

		if(EXISTS ${TEST_TXT})
			file(STRINGS ${TEST_TXT} TEST_DEPS)

			set_property(DIRECTORY APPEND PROPERTY
			CMAKE_CONFIGURE_DEPENDS "${TEST_TXT}")

			foreach(DEP ${TEST_DEPS})
				string(STRIP ${DEP} DEP)
				if(NOT DEP MATCHES "^#" AND NOT DEP STREQUAL "")
					target_link_libraries(test_${NAME} PRIVATE ${DEP})
				endif()
			endforeach()
		endif()
	endif()
endforeach()

#----------#
# for ZED! #
#----------#

set(DEBUG_TASK [=[
	{
		"label": "[$PRESET] $TARGET",
		"request": "launch",
		"adapter": "CodeLLDB",
		"build":
		{
			"args":
			[
				"--build",
				"--preset", "$PRESET",
				"--target", "test_$TARGET"
			],
			"command": "cmake",
		},
		"program": "./build/$PRESET/test_$TARGET.exe",
		"initCommands": ["command source .lldbinit"],
	},
]=])
set(DEBUG_JSON "")

foreach(ITEM gcc;msvc;clang)
	foreach(PATH ${x69_MOD})
		if(IS_DIRECTORY ${PATH})
			  get_filename_component(NAME ${PATH} NAME)
			# get_filename_component(FOLD ${PATH} PATH)

				set(TASK ${DEBUG_TASK}) #copy

				string(REPLACE "$TARGET" ${NAME}
				TASK ${TASK}) # args = out, in

				string(REPLACE "$PRESET" ${ITEM}
				TASK ${TASK}) # args = out, in

				string(APPEND DEBUG_JSON ${TASK})
		endif()
	endforeach()
endforeach()

file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/.zed/debug.json" "[\n${DEBUG_JSON}]\n")
