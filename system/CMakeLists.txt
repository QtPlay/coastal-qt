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
if(UNIX AND NOT APPLE)
    file(GLOB icons *.png)
    set(DESKTOP_PATH ${CMAKE_CURRENT_BINARY_DIR})

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libcoastal.pc.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/libcoastal.pc)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/coastal-manpager.desktop.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/coastal-manpager.desktop)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/coastal-search.desktop.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/coastal-search.desktop)

    install(FILES ${icons} DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
    install(FILES ${DESKTOP_PATH}/coastal-manpager.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES ${DESKTOP_PATH}/coastal-search.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcoastal.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

