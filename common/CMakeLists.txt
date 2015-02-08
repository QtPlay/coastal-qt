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

file(GLOB qt_src about.cpp busy.cpp coastal.cpp dialog.cpp main.cpp notify.cpp view.cpp conditional.cpp mapped.cpp)
file(GLOB qt_inc coastal.h)
file(GLOB ui_src *.ui)

option(BUILD_RUNTIME "Set to OFF to build static runtime" ON)

if(WIN32)
    set(style coastal_mingw.qrc)
else()
    set(style coastal_default.qrc)
endif()

if(BUILD_DBUS)
    QT_ADD_DBUS_INTERFACE(qt_src ../dbus/org.freedesktop.Notifications.xml dbus_notify)
endif()

if(BUILD_RUNTIME)
    set(COASTAL_LIBRARY_TYPE SHARED)
else()
    set(COASTAL_LIBRARY_TYPE STATIC)
endif()

create_languages(coastal)
create_pcfiles(libcoastal)

QT_ADD_RESOURCES(qt_qrc ${style})
QT_WRAP_CPP(qt_moc coastal.h)
QT_WRAP_UI(qt_ui ${ui_src})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(coastal ${COASTAL_LIBRARY_TYPE} ${qt_src} ${qt_inc} ${qt_moc} ${qt_ui} ${qt_qrc})
add_library_version(coastal ${LIB_VERSION})
target_link_libraries(coastal ${QT_LIBRARIES} ${COASTAL_EXTRA_LIBS})

install(TARGETS coastal DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES coastal.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

