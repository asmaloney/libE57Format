# SPDX-License-Identifier: MIT
# Copyright 2020 Andy Maloney <asmaloney@gmail.com>

target_sources( ${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/E57Format.h
		${CMAKE_CURRENT_LIST_DIR}/E57Exception.h
		${CMAKE_CURRENT_LIST_DIR}/E57Simple.h
)

install(
	FILES
		E57Format.h
		E57Exception.h
		E57Simple.h
	DESTINATION
		include/E57Format
)

target_include_directories( ${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include/E57Format>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

