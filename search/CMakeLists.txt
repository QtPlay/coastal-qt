# Copyright (C) 2011 David Sugar, Haakon Eriksen, GNU Free Call Foundation
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

file(GLOB qt_src config.cpp index.cpp view.cpp main.cpp)
file(GLOB qt_inc config.h index.h view.h main.h)
file(GLOB ui_src *.ui)
set(style search_default.qrc)

if( APPLE )
    set (PROGNAME "Coastal Search")
else()
    set (PROGNAME coastal-search)
endif()

create_rcfiles(rc_inc search)
create_languages(search)

QT_ADD_RESOURCES(qt_qrc search.qrc)
QT_WRAP_CPP(qt_moc config.h index.h view.h main.h)
QT_WRAP_UI(qt_ui ${ui_src})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROGNAME} WIN32 MACOSX_BUNDLE ${qt_src} ${qt_inc} ${qt_moc} ${qt_qrc} ${qt_ui} ${rc_inc})
target_link_libraries(${PROGNAME} coastal ${QT_LIBRARIES} ${QT_PLUGINS})
add_dependencies(${PROGNAME} coastal)
install(TARGETS ${PROGNAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

if(APPLE)
    set(SEARCH_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources)
    add_custom_command(TARGET ${PROGNAME} POST_BUILD
        COMMAND mkdir ARGS ${SEARCH_RESOURCES})
endif()

if(UNIX AND NOT APPLE)
    set(DESKTOP_PATH ${CMAKE_CURRENT_BINARY_DIR})

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/search.desktop.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/coastal-search.desktop)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/search.png
        ${CMAKE_CURRENT_BINARY_DIR}/coastal-search.png COPYONLY)

    install(FILES ${DESKTOP_PATH}/coastal-search.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
    install(FILES ${DESKTOP_PATH}/coastal-search.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

