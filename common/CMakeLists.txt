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

set (ts_common ${ts_src} ${qt_src} ${ui_src} ${qt_inc} PARENT_SCOPE)

QT4_WRAP_CPP(qt_moc coastal.h)
QT4_WRAP_UI(qt_ui ${ui_src})

if(WIN32 OR APPLE)
    set(LIBRARY_TYPE STATIC)
else()
    set(LIBRARY_TYPE SHARED)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(coastal ${LIBRARY_TYPE} ${qt_src} ${qt_inc} ${qt_moc} ${qt_ui})
set_target_properties(coastal PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})
target_link_libraries(coastal ${QT_LIBRARIES})

if(NOT WIN32)
    install(FILES coastal.h DESTINATION include)
    install(TARGETS coastal DESTINATION lib)
endif()
