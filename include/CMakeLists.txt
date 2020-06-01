# SPDX-License-Identifier: MIT
# Copyright 2020 Andy Maloney <asmaloney@gmail.com>

target_sources( ${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/E57Format.h
		${CMAKE_CURRENT_LIST_DIR}/E57Exception.h
)

install(
	FILES
		E57Format.h
		E57Exception.h
	DESTINATION
		include/E57Format
)

target_include_directories( ${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include/E57Format>
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

