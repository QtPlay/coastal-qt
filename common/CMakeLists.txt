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
set(qm_src coastal_es.ts coastal_de.ts)
set(qm_out coastal_es.qm coastal_de.qm)

if(WIN32)
    set(style coastal_mingw.qrc)
else()
    set(style coastal_default.qrc)
endif()

if(BUILD_DBUS)
    QT_ADD_DBUS_INTERFACE(qt_src ../dbus/org.freedesktop.Notifications.xml dbus_notify)
endif()

QT_ADD_RESOURCES(qt_qrc coastal.qrc)
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

add_custom_command(OUTPUT .lupdate
    DEPENDS ${qt_src} ${qt_inc} ${qt_ui} ${qm_src}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND ${QT_LUPDATE_EXECUTABLE}
        ARGS -locations none ${qt_src} ${qt_inc} ${qt_ui} -ts ${qm_src}
    COMMAND ${CMAKE_COMMAND}
        ARGS -E touch .lupdate
)

add_custom_command(OUTPUT ${qm_out}
    DEPENDS .lupdate
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND ${QT_LRELEASE_EXECUTABLE}
        ARGS ${qm_src}
)

add_custom_command(OUTPUT coastal.qrc
    DEPENDS ${style} ${qm_out}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${style} coastal.qrc
)
