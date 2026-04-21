cmake_minimum_required(VERSION 3.10)

set(CMAKE_POLICY_DEFAULT_CMP0000 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

project(x69 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_link_options("-fuse-ld=bfd")
	add_compile_options(-g -flto -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_link_options("-fuse-ld=lld")
	add_compile_options(-g -flto -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/Wall /MT /fsanitize=address /utf-8)
endif()

function(x69_register_interface PACKAGE_NAME)
	set(ROOT
		# sets project folder to "./src/${PACKAGE_NAME}"
		${CMAKE_CURRENT_SOURCE_DIR}/src/${PACKAGE_NAME})

	add_library(${PACKAGE_NAME} INTERFACE)

	add_library(${PROJECT_NAME}::${PACKAGE_NAME}
	                             ALIAS
	                             ${PACKAGE_NAME})

	target_include_directories(${PACKAGE_NAME} SYSTEM INTERFACE ${ROOT})
endfunction()

function(x69_register_unit_test PACKAGE_NAME)
	set(ROOT
		# sets project folder to "./test/${PACKAGE_NAME}/main.cpp"
		${CMAKE_CURRENT_SOURCE_DIR}/test/${PACKAGE_NAME}/main.cpp)

	add_executable(test_${PACKAGE_NAME} ${ROOT})

	add_executable(${PROJECT_NAME}::test_${PACKAGE_NAME}
	                                ALIAS
	                                test_${PACKAGE_NAME})

	# IDE specifics (Visual Studio)
	set_target_properties(test_${PACKAGE_NAME} PROPERTIES
		VS_GLOBAL_RunCodeAnalysis             true
		VS_GLOBAL_EnableMicrosoftCodeAnalysis true
		VS_GLOBAL_EnableClangTidyCodeAnalysis true
		DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)

	target_link_libraries(test_${PACKAGE_NAME} PRIVATE ${PACKAGE_NAME}
	                                                   doctest::doctest)
endfunction()

file(GLOB GIT_MOD LIST_DIRECTORIES
                  true
                  CONFIGURE_DEPENDS
                  #--------[glob ptrn]-------#
                  "${CMAKE_SOURCE_DIR}/pkg/*")

foreach(PKG_PATH ${GIT_MOD})
	  get_filename_component(PKG_NAME ${PKG_PATH} NAME)
	# get_filename_component(PKG_PATH ${PKG_PATH} PATH)

	if(IS_DIRECTORY ${PKG_PATH})
		message(STATUS "using ${PKG_NAME}")

		add_subdirectory(${PKG_PATH} SYSTEM)
	endif()
endforeach()

file(GLOB x69_MOD LIST_DIRECTORIES
                  true
                  CONFIGURE_DEPENDS
                  #--------[glob ptrn]-------#
                  "${CMAKE_SOURCE_DIR}/src/*")

foreach(PKG_PATH ${x69_MOD})
	  get_filename_component(PKG_NAME ${PKG_PATH} NAME)
	# get_filename_component(PKG_PATH ${PKG_PATH} PATH)

	if(IS_DIRECTORY ${PKG_PATH})
		message(STATUS "module ${PKG_NAME}")

		x69_register_interface(${PKG_NAME})
		x69_register_unit_test(${PKG_NAME})
	endif()
endforeach()

foreach(PKG_PATH ${x69_MOD})
	  get_filename_component(PKG_NAME ${PKG_PATH} NAME)
	# get_filename_component(PKG_PATH ${PKG_PATH} PATH)

	if(IS_DIRECTORY ${PKG_PATH})
		foreach(DEP_PATH ${x69_MOD})
			  get_filename_component(DEP_NAME ${DEP_PATH} NAME)
			# get_filename_component(DEP_PATH ${DEP_PATH} PATH)

			if(IS_DIRECTORY ${DEP_PATH})
				# don't link a library to itself
				if(NOT PKG_NAME STREQUAL DEP_NAME)
					# allows header inclusion of different x69:: submodules
					#                       <↓↓↓↓↓↓>              <↓↓↓↓↓↓>
					target_link_libraries(${PKG_NAME} INTERFACE ${DEP_NAME})
				endif()
			endif()
		endforeach()
	endif()
endforeach()
