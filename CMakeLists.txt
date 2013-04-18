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
set (VERSION 0.6.2)

set(RC_VERSION ${VERSION})
STRING(REGEX REPLACE "[.]" "," RC_VERSION ${RC_VERSION})
set(RC_VERSION "${RC_VERSION},0")

set(SOVERSION ${VERSION})
STRING(REGEX REPLACE "[.].*$" "" SOVERSION ${SOVERSION})

set (COASTAL_VERSION ${VERSION})

INCLUDE(FindPkgConfig)
INCLUDE(CheckIncludeFiles)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

if( APPLE OR WIN32)
    option(BUILD_DBUS "Set to ON to use DBUS" OFF)
else()
    option(BUILD_DBUS "Set to OFF to disable DBUS" ON)
endif()

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

if(WIN32 OR APPLE)
    set(COASTAL_LIBRARY_TYPE "STATIC")
else()
    set(COASTAL_LIBRARY_TYPE "SHARED")
endif()

if(WIN32)
    option(COASTAL_RUNTIME "Set to ON to build runtime" OFF)
    mark_as_advanced(COASTAL_RUNTIME)
    if(COASTAL_RUNTIME)
        set(COASTAL_LIBRARY_TYPE "SHARED")
        set(RUNTIME_MAGIC "${CMAKE_INSTALL_PREFIX}/misc/magic")
    endif()
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/common")
set(DEFAULT_TRANSLATIONS ${CMAKE_INSTALL_PREFIX}/${TRANSLATIONS})

# make sure qt dbus set if used
if(BUILD_DBUS)
    set(QT_USE_QTDBUS 1)
    add_definitions(-DQT_DBUS_LIB)
    include_directories(${QT_QTDBUS_INCLUDE_DIR})
endif()

MESSAGE( STATUS "Configuring ${PACKAGE} ${VERSION}...")

find_package(Qt5Core QUIET)
if(Qt5Core_FOUND)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5DBus REQUIRED)
    find_package(Qt5LinguistTools REQUIRED)
    set(QT_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5DBus_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Core_LIBRARIES})
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_PRIVATE_INCLUDE_DIRS})

    if(Qt5_POSITION_INDEPENDENT_CODE)
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()

    message(STATUS "Building with QT5 Support")

    macro(qt_add_dbus_interface)
        qt5_add_dbus_interface(${ARGN})
    endmacro()

    macro(qt_create_translation)
        qt5_create_translation(${ARGN})
    endmacro()

    macro(qt_add_resources)
        qt5_add_resources(${ARGN})
    endmacro()

    macro(qt_wrap_cpp)
        qt5_wrap_cpp(${ARGN})
    endmacro()

    macro(qt_wrap_ui)
        qt5_wrap_ui(${ARGN})
    endmacro()

else()
    macro(qt_add_dbus_interface)
        qt4_add_dbus_interface(${ARGN})
    endmacro()

    macro(qt_add_resources)
        qt4_add_resources(${ARGN})
    endmacro()

    macro(qt_create_translation)
        qt4_create_translation(${ARGN})
    endmacro()

    macro(qt_wrap_cpp)
        qt4_wrap_cpp(${ARGN})
    endmacro()

    macro(qt_wrap_ui)
        qt4_wrap_ui(${ARGN})
    endmacro()

    find_package(Qt4 4.8.0 COMPONENTS QtCore QtGui REQUIRED)
    if(QT4_FOUND)
        if(QTVERSION VERSION_GREATER "4.99.99")
            set(QT5_FOUND true)
        endif()
        include(${QT_USE_FILE})
        ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT_NETWORK_LIB)
        include_directories(${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}/../QtWidgets ${QT_QTGUI_INCLUDE_DIR}/${QTVERSION}/QtGui)
        if(QT_IS_STATIC)
            file(GLOB QT_PLUGINS ${QT_PLUGINS_DIR}/codecs/*${CMAKE_STATIC_LIBRARY_SUFFIX})
        endif()

    else()
        message(ERROR "Building without QT")
    endif()
endif()

if(UNIX AND NOT APPLE)
    find_package(X11)
    if(X11_FOUND)
        include_directories(${X11_INCLUDE_DIRS})
        check_include_files(X11/extensions/scrnsaver.h HAVE_LIBXSS)
    endif()
endif()

check_include_files(windows.h HAVE_WINDOWS_H)
check_include_files(magic.h HAVE_MAGIC)
check_include_files(magic.h HAVE_UNISTD_H)

if(HAVE_MAGIC)
    set(COASTAL_EXTRA_LIBS ${COASTAL_EXTRA_LIBS} magic)
    if(WIN32)
        set(COASTAL_EXTRA_LIBS ${COASTAL_EXTRA_LIBS} regex shlwapi)
    endif()
endif()

if(HAVE_LIBXSS)
    set(COASTAL_EXTRA_LIBS ${COASTAL_EXTRA_LIBS} Xss ${X11_LIBRARIES})
endif()

configure_file(coastal-qt-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/coastal-qt-config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE}.spec.cmake ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE}.spec @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(QT5_FOUND)
    find_library(QT5_WIDGETS NAMES QtWidgets HINTS ${QT_LIBRARY_DIR})
    set(QT_LIBRARIES ${QT_LIBRARIES} ${QT5_WIDGETS})
endif()

add_subdirectory(common)
add_subdirectory(manpager)
add_subdirectory(search)
add_subdirectory(notify)
add_subdirectory(dialogs)
add_subdirectory(system)
add_subdirectory(translations)

if(CMAKE_GENERATOR MATCHES "Unix Makefiles")
    add_custom_target(cleandist
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}[-_]*.gz"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}_*.dsc"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}-*.rpm"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}[-_]*.deb"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/*coastal*[-_]*.deb"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}-*.zip"
        COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/*coastal*.changes"
    )

    add_custom_target(dist
        DEPENDS cleandist
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND git archive --format tar --output="${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}-${VERSION}.tar" --prefix="${PACKAGE}-${VERSION}/" HEAD
        COMMAND gzip "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}-${VERSION}.tar"
    )
endif()

