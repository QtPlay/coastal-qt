#.rst:
# CoastalConfig
# ---------------

#=============================================================================
# Copyright (C) 2015 David Sugar, Tycho Softworks.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#=============================================================================

include(GNUInstallDirs)
include(CheckFunctionExists)
include(CheckIncludeFiles)

macro(check_functions)
    foreach(arg ${ARGN})
        string(TOUPPER "${arg}" _fn)
        set(_fn "HAVE_${_fn}")
        check_function_exists(${arg} ${_fn})
    endforeach()
endmacro()

macro(check_headers)
    foreach(arg ${ARGN})
        string(TOUPPER "${arg}" _hdr)
        string(REGEX REPLACE "/" "_" _hdr ${_hdr})
        string(REGEX REPLACE "[.]" "_" _hdr ${_hdr})
        set(_hdr "HAVE_${_hdr}")
        check_include_files(${arg} ${_hdr})
    endforeach()
endmacro()

macro(target_setuid_properties)
    if(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_COMPILE_SETUID_FLAGS)
	    set(CMAKE_COMPILE_SETUID_FLAGS "-O2 -fPIE -fstack-protector -D_FORTIFY_SOURCE=2 --param ssp-buffer-size=4 -pie") 
        set(CMAKE_LINK_SETUID_FLAGS "-pie -z relro -z now")
    endif()
    foreach(arg ${ARGN})
        set_target_properties(${arg} PROPERTIES COMPILE_FLAGS "${CMAKE_COMPILE_SETUID_FLAGS}" LINK_FLAGS "${CMAKE_LINK_SETUID_FLAGS}" POSITION_INDEPENDENT_CODE TRUE )
    endforeach()
endmacro()

function(set_library_version _LIBRARY)
    set(_VERSION "${ARGN}")
    set(_SOVERSION "-")
	if(NOT UNIX OR "${_VERSION}" STREQUAL "")
		set(_VERSION "${VERSION}")
	endif()
	STRING(REGEX REPLACE "[.].*$" "" _SOVERSION ${_VERSION})
	if(ABI_MAJORONLY)
		set(_VERSION ${_SOVERSION})
	endif()
    set_target_properties(${_LIBRARY} PROPERTIES VERSION ${_VERSION} SOVERSION ${_SOVERSION})
endfunction()

function(set_source_dependencies _TARGET)
    if(NOT "${CMAKE_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
        add_dependencies(${_TARGET} ${ARGN})
    endif()
endfunction()

function(create_specfile)
    if(ARGN)
        foreach(arg ${ARGN})
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${arg}.spec.cmake ${CMAKE_CURRENT_SOURCE_DIR}/${arg}.spec @ONLY)
        endforeach()
    else()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.spec.cmake ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.spec @ONLY)
    endif()
endfunction()

function(create_rcfiles _OUTPUT)
    set(RC_VERSION ${VERSION})
	string(REGEX REPLACE "[.]" "," RC_VERSION ${VERSION})
	set(RC_VERSION "${RC_VERSION},0")
    set(RC_FILES)

    if(ARGN)
        foreach(arg ${ARGN})
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${arg}.rc.cmake ${CMAKE_CURRENT_SOURCE_DIR}/${arg}.rc)
            set(RC_FILES ${RC_FILES} ${arg}.rc)
        endforeach()
    else()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc.cmake ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc)
        set(RC_FILES ${PROJECT_NAME}.rc)
    endif()
    if(WIN32)
        set(${_OUTPUT} ${RC_FILES} PARENT_SCOPE)
    else()
        set(${_OUTPUT} PARENT_SCOPE)
    endif()
endfunction()

function(create_scripts _OUTPUT)
    set(SCR_FILES)
    foreach(arg ${ARGN})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${arg}.cmake ${CMAKE_CURRENT_BINARY_DIR}/${arg})
        set(SCR_FILES ${SCR_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${arg})
    endforeach()
    set(${_OUTPUT} ${SCR_FILES} PARENT_SCOPE)
endfunction()

function(create_pcfiles _OUTPUT)
    set(PC_FILES)
    if(ARGN)
        foreach(arg ${ARGN})
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${arg}.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${arg}.pc)
            set(PC_FILES ${PC_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${arg}.pc)
        endforeach()
    else()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc)
        set(PC_FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc)
    endif()
    set(${_OUTPUT} ${PC_FILES} PARENT_SCOPE)
endfunction()

function(create_headers)
    if(ARGN)
        foreach(arg ${ARGN})
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${arg}.cmake ${CMAKE_CURRENT_BINARY_DIR}/${arg})
        endforeach()
    else()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.h)
    endif()
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endfunction()

macro(create_languages _NAME)
	if(Qt5Core_FOUND)
        find_package(Qt5LinguistTools REQUIRED)
	endif()

	file(GLOB _TSFILES ${_NAME}_*.ts)

	execute_process(
		COMMAND ${QT_LRELEASE_EXECUTABLE} -silent ${_TSFILES}
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	)

	add_custom_target(lupdate-${_NAME}
		DEPENDS ${ARGN} ${_TSFILES}
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		COMMAND ${QT_LUPDATE_EXECUTABLE} -locations none ${qt_src} ${qt_inc} ${qt_ui} -ts ${qm_src}
		COMMAND ${CMAKE_COMMAND} -E touch manpager.qrc
	)
endmacro()

# 5, 4, or any...
macro(qt_config_core_library _ABI)
    if("${_ABI}" STREQUAL "5")
        set(BUILD_QT4ONLY OFF CACHE BOOL "qt5 only" FORCE)
        find_package(Qt5Core REQUIRED)
    else()
        if("${_ABI}" STREQUAL "4")
            set(BUILD_QT4ONLY ON CACHE BOOL "qt4 only" FORCE)
        else()
            option(BUILD_QT4ONLY "Set to ON to ignore Qt5" OFF)
            if(NOT BUILD_QT4ONLY)
		        find_package(Qt5Core QUIET)
	        endif()
        endif()
    endif()
	if(Qt5Core_FOUND)
        set(QT_CONFIG_CORE_LIBRARY "5")
		set(QT_LIBRARIES ${Qt5Core_LIBRARIES})
		include_directories(${Qt5Core_INCLUDE_DIRS})
    
		if(Qt5_POSITION_INDEPENDENT_CODE)
			set(CMAKE_POSITION_INDEPENDENT_CODE ON)
		endif()

		FIND_PROGRAM(QT_LUPDATE_EXECUTABLE NAMES lupdate-qt5 lupdate PATHS
			$ENV{QTDIR}/bin
		)

		FIND_PROGRAM(QT_LRELEASE_EXECUTABLE NAMES lrelease-qt5 lrelease PATHS
			$ENV{QTDIR}/bin
		)

	else()
        set(QT_CONFIG_CORE_LIBRARY "4")
		find_package(Qt4 4.8.0 COMPONENTS QtCore REQUIRED)

        FIND_PROGRAM(QT_LUPDATE_EXECUTABLE NAMES lupdate-qt4 lupdate PATHS
			$ENV{QTDIR}/bin
		)

		FIND_PROGRAM(QT_LRELEASE_EXECUTABLE NAMES lrelease-qt4 lrelease PATHS
			$ENV{QTDIR}/bin
		)

        include(${QT_USE_FILE})
		add_definitions(-DQT_CORE_LIB)
        include_directories(${QT_QTCORE_INCLUDE_DIR})

	endif()
endmacro()

macro(qt_config_gui_widgets)
    if(Qt5Core_FOUND)
        find_package(Qt5Gui REQUIRED)
        find_package(Qt5Widgets REQUIRED)
        set(QT_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} ${QT_LIBRARIES})
        include_directories(${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
    else()
        find_package(Qt4 4.8.0 COMPONENTS QtGui REQUIRED)
        set(QT_LIBRARIES ${QT_LIBRARIES} ${QT_QTGUI_LIBRARIES})
        add_definitions(-DQT_GUI_LIBS)
        include_directories(${QT_QTGUI_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}/../QtWidgets ${QT_QTGUI_INCLUDE_DIR}/${QTVERSION}/QtGui)

        if(QT_IS_STATIC)
            file(GLOB QT_PLUGINS ${QT_PLUGINS_DIR}/codecs/*${CMAKE_STATIC_LIBRARY_SUFFIX})
        endif()
    endif()
endmacro()

macro(qt_config_dbus_support)
    if( APPLE OR WIN32 OR MINGW)
        option(BUILD_DBUS "Set to ON to use DBUS" OFF)
    else()
        option(BUILD_DBUS "Set to OFF to disable DBUS" ON)
    endif()

    if(BUILD_DBUS)
        set(QT_USE_QTDBUS 1)
        if(Qt5Core_FOUND)
            find_package(Qt5DBus REQUIRED)
            set(QT_LIBRARIES ${Qt5DBus_LIBRARIES} ${QT_LIBRARIES})
            include_directories(${Qt5DBus_INCLUDE_DIRS})
        else()
            find_package(Qt4 4.8.0 COMPONENTS QtDBus REQUIRED)
            set(QT_LIBRARIES ${QT_QTDBUS_LIBRARIES} ${QT_LIBRARIES})
            add_definitions(-DQT_DBUS_LIB)
            include_directories(${QT_QTDBUS_INCLUDE_DIR})
        endif()
    endif()
endmacro()

macro(qt_config_coastal_macros)
    include(CoastalMacros)
endmacro()

