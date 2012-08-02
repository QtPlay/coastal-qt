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

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set (PROGNAME coastal-notify)

add_executable(${PROGNAME} ${qt_src} ${qt_inc})
target_link_libraries(${PROGNAME} coastal ${QT_LIBRARIES} ${QT_PLUGINS})
add_dependencies(${PROGNAME} coastal)
install(TARGETS ${PROGNAME} DESTINATION ${INSTALL_BINDIR})

