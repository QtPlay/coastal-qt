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
    set(MANPAGER_DESKTOP coastal-manpager.desktop)

    set(ICON_FILES ../icons/coastal-manpager.png)
    set(DESKTOP_FILES ${MANPAGER_DESKTOP})

    add_custom_command(OUTPUT ${MANPAGER_DESKTOP}
        COMMAND touch ${MANPAGER_DESKTOP}
        COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/manpager-desktop.sh ${CMAKE_INSTALL_PREFIX} >${MANPAGER_DESKTOP}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/manpager-desktop.sh
        COMMENT "Generating desktop entry file"
    )

    add_custom_target(BUILD_DESKTOP_FILES ALL
        DEPENDS ${DESKTOP_FILES}
    )

    install(FILES ${ICON_FILES} DESTINATION share/icons)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILES} DESTINATION share/applications)
endif()

