# SPDX-License-Identifier: MIT
# Copyright 2022 Andy Maloney <asmaloney@gmail.com>

target_sources( ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RandomNum.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TestData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_SimpleData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_SimpleReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_SimpleWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_StringFunctions.cpp
)
