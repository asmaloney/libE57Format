# SPDX-License-Identifier: MIT
# Copyright 2022 Andy Maloney <asmaloney@gmail.com>

project( testE57
    LANGUAGES
        CXX
)

add_executable( testE57 )

add_subdirectory( include )
add_subdirectory( src )

set_target_properties( testE57
	PROPERTIES
	    CXX_STANDARD 14
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
		POSITION_INDEPENDENT_CODE ON
)

target_compile_options( testE57
	PUBLIC
	    # add switches to MSVC for UTF-8 handling in our test files
		$<$<C_COMPILER_ID:MSVC>:/utf-8>
		$<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

# ccache
# https://crascit.com/2016/04/09/using-ccache-with-cmake/
find_program( CCACHE_PROGRAM ccache )
if ( CCACHE_PROGRAM )
    message( STATUS "[E57 Test] Using ccache" )
    set_target_properties( testE57
        PROPERTIES
            CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}"
            C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}"
    )
endif()

target_include_directories( testE57
    PRIVATE
        ../src
)

target_link_libraries( testE57
    PRIVATE
        E57Format
)

# Turn on ASAN
target_link_libraries( testE57
    PRIVATE
        $<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>
        $<$<CONFIG:DEBUG>:-fsanitize=address>
)

# Test data
# Find our test data and offer an option to set the path to it
find_path( E57_TEST_DATA_PATH
    NAMES libE57Format-test-data
    PATHS
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/..
        ${PROJECT_SOURCE_DIR}/../..
    NO_DEFAULT_PATH
    DOC "Path to directory containing the libE57Format-test-data repository"
)

if ( E57_TEST_DATA_PATH )
    message( STATUS "[E57 Test] Using test data from: ${E57_TEST_DATA_PATH}/libE57Format-test-data")

    target_compile_definitions( testE57
        PRIVATE
            TEST_DATA_PATH="${E57_TEST_DATA_PATH}/libE57Format-test-data"
    )
else()
    message( WARNING "[E57 Test] Test data not found. Please set E57_TEST_DATA_PATH to the directory containing libE57Format-test-data.")
endif()

# GoogleTest from here: https://github.com/google/googletest

if ( NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest/CMakeLists.txt" )
    message( FATAL_ERROR "[E57 Test] The GoogleTest submodule was not downloaded. E57_GIT_SUBMODULE_UPDATE was turned off or failed. Please update submodules and try again." )
endif()

set( BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject" )
set( INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" )

if ( MSVC )
    # Prevent overriding the parent project's compiler/linker settings on Windows
    set( gtest_force_shared_crt ON CACHE BOOL "" FORCE )
endif()

add_subdirectory( extern/googletest )

unset( BUILD_GMOCK CACHE )
unset( INSTALL_GTEST CACHE )

target_include_directories( testE57
    PRIVATE
        "${gtest_SOURCE_DIR}/include"
)

target_link_libraries( testE57
    PRIVATE
        gtest_main
)
