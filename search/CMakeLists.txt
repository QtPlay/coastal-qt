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
file(GLOB rc_src *.qrc)

set (ts_search ${ts_common} ${qt_src} ${ui_src} ${qt_inc} PARENT_SCOPE)
set (ts_out ../translations/coastal-search.ts)

QT4_ADD_RESOURCES(qt_qrc ${rc_src})
QT4_WRAP_CPP(qt_moc program.h)
QT4_WRAP_UI(qt_ui ${ui_src})

add_executable(coastal-search WIN32 ${qt_src} ${qt_inc} ${qt_moc} ${qt_qrc} ${qt_ui} ${qm_out})
target_link_libraries(coastal-search coastal ${QT_LIBRARIES})
add_dependencies(coastal-search coastal)
install(TARGETS coastal-search DESTINATION ${INSTALL_BINDIR})
