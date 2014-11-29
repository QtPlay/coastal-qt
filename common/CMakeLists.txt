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

file(GLOB qt_src about.cpp busy.cpp coastal.cpp dialog.cpp main.cpp notify.cpp view.cpp conditional.cpp)
file(GLOB qt_inc coastal.h)
file(GLOB ui_src *.ui)

if(WIN32)
    set(rc_src mingw.qrc)
else()
    set(rc_src coastal.qrc)
endif()

set (ts_common ${ts_src} ${qt_src} ${ui_src} ${qt_inc} PARENT_SCOPE)

if(BUILD_DBUS)
    QT_ADD_DBUS_INTERFACE(qt_src ../dbus/org.freedesktop.Notifications.xml dbus_notify)
endif()

QT_CREATE_TRANSLATION(qm_libcoastal ${ts_libcoastal} libcoastal.ts)
QT_ADD_RESOURCES(qt_qrc ${rc_src})
QT_WRAP_CPP(qt_moc coastal.h)
QT_WRAP_UI(qt_ui ${ui_src})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libcoastal.pc.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/libcoastal.pc)


add_library(coastal ${COASTAL_LIBRARY_TYPE} ${qt_src} ${qt_inc} ${qt_moc} ${qt_ui} ${qt_qrc})
set_target_properties(coastal PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})
target_link_libraries(coastal ${QT_LIBRARIES} ${COASTAL_EXTRA_LIBS})

install(TARGETS coastal DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES coastal.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcoastal.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

if(CMAKE_GENERATOR MATCHES "Unix Makefiles")
    add_custom_target(translate
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND lupdate -locations none ${qt_src} ${qt_inc} ${qt_ui} -ts ${CMAKE_CURRENT_SOURCE_DIR}/coastal.ts
    )
endif()

