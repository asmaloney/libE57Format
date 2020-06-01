# SPDX-License-Identifier: MIT
# Copyright 2020 Andy Maloney <asmaloney@gmail.com>

target_sources( E57Format
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/CheckedFile.h
        ${CMAKE_CURRENT_LIST_DIR}/CheckedFile.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Common.h
        ${CMAKE_CURRENT_LIST_DIR}/Decoder.h
        ${CMAKE_CURRENT_LIST_DIR}/Decoder.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Encoder.h
        ${CMAKE_CURRENT_LIST_DIR}/Encoder.cpp
        ${CMAKE_CURRENT_LIST_DIR}/NodeImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/NodeImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Packet.h
        ${CMAKE_CURRENT_LIST_DIR}/Packet.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ImageFileImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ImageFileImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/SourceDestBufferImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/SourceDestBufferImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/StructureNodeImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/StructureNodeImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57Exception.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57Format.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57FormatImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57FormatImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/E57Version.h
        ${CMAKE_CURRENT_LIST_DIR}/E57XmlParser.h
        ${CMAKE_CURRENT_LIST_DIR}/E57XmlParser.cpp

)

target_include_directories( E57Format
	PRIVATE
	    ${CMAKE_CURRENT_SOURCE_DIR}
)
