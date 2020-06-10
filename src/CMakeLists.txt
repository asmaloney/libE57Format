# SPDX-License-Identifier: MIT
# Copyright 2020 Andy Maloney <asmaloney@gmail.com>

target_sources( E57Format
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/CheckedFile.h
        ${CMAKE_CURRENT_LIST_DIR}/CheckedFile.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Common.h
        ${CMAKE_CURRENT_LIST_DIR}/Common.cpp
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
        ${CMAKE_CURRENT_LIST_DIR}/ReaderImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ReaderImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/SourceDestBufferImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/SourceDestBufferImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/StructureNodeImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/StructureNodeImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/WriterImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/WriterImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/E57Exception.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57Format.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57FormatImpl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57FormatImpl.h
        ${CMAKE_CURRENT_LIST_DIR}/E57SimpleData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57SimpleReader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57SimpleWriter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57Version.h
        ${CMAKE_CURRENT_LIST_DIR}/E57XmlParser.cpp
        ${CMAKE_CURRENT_LIST_DIR}/E57XmlParser.h
)

target_include_directories( E57Format
	PRIVATE
	    ${CMAKE_CURRENT_SOURCE_DIR}
)
