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

file(GLOB qt_src main.cpp index.cpp validate.cpp view.cpp config.cpp)
file(GLOB qt_inc main.h index.h validate.h view.h config.h)
file(GLOB ui_src *.ui)
file(GLOB rc_src *.qrc)

if( APPLE )
    set(PROGNAME "Coastal Manpager")
else()
    set(PROGNAME coastal-manpager)
endif()

set (ts_manpager ${ts_common} ${qt_src} ${ui_src} ${qt_inc} PARENT_SCOPE)

configure_file(manpager.rc.cmake ${CMAKE_CURRENT_SOURCE_DIR}/manpager.rc)

QT_CREATE_TRANSLATION(qm_manpager ${ts_manpager} manpager.ts)
QT_ADD_RESOURCES(qt_qrc ${rc_src})
QT_WRAP_CPP(qt_moc main.h index.h view.h config.h)
QT_WRAP_UI(qt_ui ${ui_src})

if(WIN32)
    set(qt_qrc ${qt_qrc} manpager.rc)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROGNAME} WIN32 MACOSX_BUNDLE ${qt_src} ${qt_inc} ${qt_moc} ${qt_qrc} ${qt_ui})
target_link_libraries(${PROGNAME} coastal ${QT_LIBRARIES} ${QT_PLUGINS})
add_dependencies(${PROGNAME} coastal)
install(TARGETS ${PROGNAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

if(APPLE)
    set(MANPAGER_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources)
    add_custom_command(TARGET ${PROGNAME} POST_BUILD
        COMMAND mkdir ARGS ${MANPAGER_RESOURCES})
endif()

if(UNIX AND NOT APPLE)
    set(DESKTOP_PATH ${CMAKE_CURRENT_BINARY_DIR})

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/manpager.desktop.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/coastal-manpager.desktop)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/manpager.png 
        ${CMAKE_CURRENT_BINARY_DIR}/coastal-manpager.png COPYONLY)

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/coastal-manpager.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
    install(FILES ${DESKTOP_PATH}/coastal-manpager.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

if(CMAKE_GENERATOR MATCHES "Unix Makefiles")
    add_custom_target(translate-manpager
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND lupdate -locations none ${qt_src} ${qt_inc} ${qt_ui} -ts ${CMAKE_CURRENT_SOURCE_DIR}/manpager.ts
    )
endif()

