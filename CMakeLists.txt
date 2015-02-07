# Copyright (C) 2011-2015 David Sugar, Tycho Softworks
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
set (VERSION 0.10.0)
set (LIB_VERSION 0.0.10)

INCLUDE(FindPkgConfig)
INCLUDE(CheckIncludeFiles)
INCLUDE(GNUInstallDirs)
INCLUDE(CapeMakeTargets)
INCLUDE(CoastalConfig)

if(WIN32)
    set(COASTAL_LIBRARY_TYPE "STATIC")
else()
    set(COASTAL_LIBRARY_TYPE "SHARED")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/common")

qt_config_core_library(any)
qt_config_dbus_support()
qt_config_gui_widgets()
qt_config_coastal_macros()

MESSAGE( STATUS "Configuring ${PACKAGE} ${VERSION}...")

if(QT_CONFIG_CORE_LIBRARY STREQUAL "4")
    check_include_files(magic.h HAVE_MAGIC)
    if(HAVE_MAGIC)
        set(COASTAL_EXTRA_LIBS ${COASTAL_EXTRA_LIBS} magic)
        if(WIN32)
            set(COASTAL_EXTRA_LIBS ${COASTAL_EXTRA_LIBS} regex shlwapi)
            set(RUNTIME_MAGIC ${CMAKE_INSTALL_FULL_SYSCONFDIR}/magic)
        endif()
    endif()
endif()

if(UNIX AND NOT APPLE)
    find_package(X11)
    if(X11_FOUND)
        include_directories(${X11_INCLUDE_DIRS})
        check_include_files(X11/extensions/scrnsaver.h HAVE_LIBXSS)
    endif()
endif()

if(HAVE_LIBXSS)
    set(COASTAL_EXTRA_LIBS ${COASTAL_EXTRA_LIBS} Xss ${X11_LIBRARIES})
endif()

check_headers(windows.h unistd.h)
create_headers()
create_specfile()

add_subdirectory(common)
add_subdirectory(manpager)
add_subdirectory(search)
add_subdirectory(dialogs)

add_cape_make_targets(coastal-qt ${VERSION})

