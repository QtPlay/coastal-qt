#.rst:
# CoastalMacros
# ---------------

#=============================================================================
# Copyright (C) 2015 David Sugar, Tycho Softworks.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#=============================================================================

if(QT_CONFIG_CORE_LIBRARY STREQUAL "4")
    macro(qt_add_dbus_interface)
        qt4_add_dbus_interface(${ARGN})
    endmacro()

    macro(qt_add_resources)
        qt4_add_resources(${ARGN})
    endmacro()

    macro(qt_wrap_cpp)
        qt4_wrap_cpp(${ARGN})
    endmacro()

    macro(qt_wrap_ui)
        qt4_wrap_ui(${ARGN})
    endmacro()
else()
    macro(qt_add_dbus_interface)
        qt5_add_dbus_interface(${ARGN})
    endmacro()

    macro(qt_add_resources)
        qt5_add_resources(${ARGN})
    endmacro()

    macro(qt_wrap_cpp)
        qt5_wrap_cpp(${ARGN})
    endmacro()

    macro(qt_wrap_ui)
        qt5_wrap_ui(${ARGN})
    endmacro()
endif()

