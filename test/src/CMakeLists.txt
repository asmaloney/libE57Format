# SPDX-License-Identifier: MIT
# Copyright 2022 Andy Maloney <asmaloney@gmail.com>

target_sources( ${PROJECT_NAME}
    PRIVATE
        main.cpp
        RandomNum.cpp
        TestData.cpp
        test_SimpleData.cpp
        test_SimpleReader.cpp
        test_SimpleWriter.cpp
)

# Include internal tests if not building shared lib on Windows.
# The functions aren't exported in the Windows DLLs.
if ( (NOT WIN32) OR (WIN32 AND NOT E57_BUILD_SHARED) )
    target_sources( ${PROJECT_NAME}
        PRIVATE
           test_StringFunctions.cpp
    )
endif()
