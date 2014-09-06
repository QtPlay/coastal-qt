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
file(GLOB qt_inc *.h)
file(GLOB ui_src *.ui)

if(WIN32)
    set(rc_src mingw.qrc)
else()
    set(rc_src coastal.qrc)
endif()

list(REMOVE_ITEM qt_src ${CMAKE_CURRENT_SOURCE_DIR}/dbus_notify.cpp)
list(REMOVE_ITEM qt_src ${CMAKE_CURRENT_SOURCE_DIR}/dbus_notify.h)

set (ts_common ${ts_src} ${qt_src} ${ui_src} ${qt_inc} PARENT_SCOPE)

if(BUILD_DBUS)
    QT_ADD_DBUS_INTERFACE(qt_src ../dbus/org.freedesktop.Notifications.xml dbus_notify)
endif()

QT_ADD_RESOURCES(qt_qrc ${rc_src})
QT_WRAP_CPP(qt_moc coastal.h)
QT_WRAP_UI(qt_ui ${ui_src})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(coastal ${COASTAL_LIBRARY_TYPE} ${qt_src} ${qt_inc} ${qt_moc} ${qt_ui} ${qt_qrc})
set_target_properties(coastal PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})
target_link_libraries(coastal ${QT_LIBRARIES} ${COASTAL_EXTRA_LIBS})

IF(WIN32)
    install(TARGETS coastal DESTINATION bin)    # dll's kept with bin
else()
    install(FILES coastal.h DESTINATION ${INSTALL_INCLUDEDIR})
    install(TARGETS coastal DESTINATION ${INSTALL_LIBDIR})
endif()
