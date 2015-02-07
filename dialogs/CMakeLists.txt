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

file(GLOB qt_src *.cpp)
file(GLOB qt_inc program.h)
file(GLOB ui_src *.ui)

QT_ADD_RESOURCES(dialog_qrc dialog.qrc)
QT_WRAP_CPP(icons_moc icons.h)
QT_WRAP_CPP(dialog_moc dialog.h)
QT_WRAP_UI(icons_ui icons.ui)

#if(WIN32)
#    set(qt_qrc ${qt_qrc} application.rc)
#endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set (ICONS_PROGNAME coastal-icons)
set (DIALOG_PROGNAME coastal-dialog)

add_executable(${ICONS_PROGNAME} WIN32 MACOSX_BUNDLE icons.cpp icons.h ${icons_moc} ${icons_ui})
target_link_libraries(${ICONS_PROGNAME} coastal ${QT_LIBRARIES} ${QT_PLUGINS})
add_dependencies(${ICONS_PROGNAME} coastal)
install(TARGETS ${ICONS_PROGNAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(${DIALOG_PROGNAME} WIN32 MACOSX_BUNDLE dialog.cpp dialog.h ${dialog_moc} ${dialog_qrc})
target_link_libraries(${DIALOG_PROGNAME} coastal ${QT_LIBRARIES} ${QT_PLUGINS})
add_dependencies(${DIALOG_PROGNAME} coastal)
install(TARGETS ${DIALOG_PROGNAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

create_languages(dialog)

