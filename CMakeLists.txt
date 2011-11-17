# Copyright (C) 2011 David Sugar, Tycho Softworks
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

cmake_minimum_required(VERSION 2.6)
PROJECT("Coastal Qt")
INCLUDE(FindPkgConfig)
INCLUDE(CheckIncludeFiles)
set (VERSION 0.1.0)
set (SOVERSION 0)

set(INSTALL_BINDIR bin)
if(WIN32 AND NOT MSYS AND NOT CYGWIN AND NOT MINGW)
    set(TRANSLATIONS translations)
else()
    set(TRANSLATIONS share/coastal-qt/translations)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../frameworks")
    file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../frameworks" LOCAL_FRAMEWORKS)
    file(TO_CMAKE_PATH "${LOCAL_FRAMEWORKS}/include" LOCAL_INCLUDES)
    file(TO_CMAKE_PATH "${LOCAL_FRAMEWORKS}/${CMAKE_BUILD_TYPE}" LOCAL_LIBS)
    set(CMAKE_REQUIRED_INCLUDES ${LOCAL_INCLUDES} ${CMAKE_REQUIRED_INCLUDES})
    link_directories(${LOCAL_LIBS})
    include_directories(${LOCAL_INCLUDES} "${CMAKE_CURRENT_SOURCE_DIR}/common")
else()
    include_directories("${CMAKE_CURRENT_SOURCR_DIR}/common")
endif()

if(LOCAL_FRAMEWORKS AND EXISTS "${LOCAL_FRAMEWORKS}/qt")
    file(TO_CMAKE_PATH "${LOCAL_FRAMEWORKS}/qt" LOCAL_QT)
    set(CMAKE_PREFIX_PATH ${LOCAL_QT} ${CMAKE_PREFIX_PATH})
endif()

set(DEFAULT_TRANSLATIONS ${CMAKE_INSTALL_PREFIX}/${TRANSLATIONS})

find_package(Qt4)
if(QT4_FOUND)
    include(${QT_USE_FILE})
    ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB)
else()
    message(ERROR "Building without QT")
endif()

check_include_files(windows.h HAVE_WINDOWS_H)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(common)
add_subdirectory(manpager)
add_subdirectory(search)

