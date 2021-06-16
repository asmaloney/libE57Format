# SPDX-License-Identifier: MIT
# Copyright 2020 Andy Maloney <asmaloney@gmail.com>

project(Examples)

set( APP_EXE ReadImages )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(${APP_EXE} ReadImages.cpp)
target_link_libraries(${APP_EXE} PUBLIC E57Format)
