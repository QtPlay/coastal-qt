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
PROJECT(coastal-qt)
set (PACKAGE coastal-qt)
set (VERSION 0.2.1)
set (SOVERSION 0)

set (COASTAL_VERSION ${VERSION})

INCLUDE(FindPkgConfig)
INCLUDE(CheckIncludeFiles)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

if( APPLE )
    SET( MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION} )
    SET( MACOSX_BUNDLE_VERSION ${VERSION} )
    SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version ${VERSION} )
    if(NOT CMAKE_OSX_ARCHITECTURES)
        SET( CMAKE_OSX_ARCHITECTURES ppc;i386 )
    endif()
endif()

set(INSTALL_BINDIR bin)
if(WIN32 AND NOT MSYS AND NOT CYGWIN AND NOT MINGW)
    set(TRANSLATIONS translations)
else()
    set(TRANSLATIONS share/coastal-qt/translations)
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/common")
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
configure_file(config.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE}.spec.cmake ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE}.spec @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(common)
add_subdirectory(manpager)
add_subdirectory(search)
add_subdirectory(system)
add_subdirectory(translations)

if(CMAKE_GENERATOR MATCHES "Unix Makefiles")
    add_custom_target(cleandist
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}[-_]*.gz"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}_*.dsc"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}-*.rpm"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}[-_]*.deb"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}-*.zip"
    )

    add_custom_target(dist
        DEPENDS cleandist
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND git archive --format tar --output="${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}-${VERSION}.tar" --prefix="${PACKAGE}-${VERSION}/" HEAD
        COMMAND gzip "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}-${VERSION}.tar"
    )
endif()

