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
file(GLOB rc_src *.qrc)

if( APPLE )
    set (PROGNAME "Coastal Search")
else()
    set (PROGNAME coastal-search)
endif()

set (ts_search ${ts_common} ${qt_src} ${ui_src} ${qt_inc} PARENT_SCOPE)
set (ts_out ../translations/coastal-search.ts)

configure_file(search.rc.cmake ${CMAKE_CURRENT_SOURCE_DIR}/search.rc)

QT_CREATE_TRANSLATION(qm_search ${ts_search} search.ts)
QT_ADD_RESOURCES(qt_qrc ${rc_src})
QT_WRAP_CPP(qt_moc config.h index.h view.h main.h)
QT_WRAP_UI(qt_ui ${ui_src})

if(WIN32)
    set(qt_qrc ${qt_qrc} search.rc)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROGNAME} WIN32 MACOSX_BUNDLE ${qt_src} ${qt_inc} ${qt_moc} ${qt_qrc} ${qt_ui} ${qm_out})
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
        ${CMAKE_CURRENT_BINARY_DIR}/search.desktop)

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/search.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps/coastal-search.png)
    install(FILES ${DESKTOP_PATH}/search.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications/coastal-search.desktop)
endif()

if(CMAKE_GENERATOR MATCHES "Unix Makefiles")
    add_custom_target(translate-search
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND lupdate -locations none ${qt_src} ${qt_inc} ${qt_ui} -ts ${CMAKE_CURRENT_SOURCE_DIR}/search.ts
    )
endif()

