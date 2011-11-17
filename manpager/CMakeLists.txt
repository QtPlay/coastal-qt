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

file(GLOB qt_src *.cpp)
file(GLOB qt_inc *.h)
file(GLOB ui_src *.ui)
file(GLOB rc_src *.qrc)

QT4_ADD_RESOURCES(qt_qrc ${rc_src})
QT4_WRAP_CPP(qt_moc program.h)
QT4_WRAP_UI(qt_ui ${ui_src})

add_executable(coastal-manpager ${qt_src} ${qt_inc} ${qt_moc} ${qt_qrc} ${qt_ui})
target_link_libraries(coastal-manpager ${QT_LIBRARIES})
install(TARGETS coastal-manpager DESTINATION ${INSTALL_BINDIR})

