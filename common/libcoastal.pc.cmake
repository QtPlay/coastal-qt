prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_FULL_LIBDIR}
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}

Name: libcoastal
Description: coastal library
Version: ${VERSION}
Libs: -lcoastal -L${CMAKE_INSTALL_FULL_LIBDIR}
CFlags: -I${CMAKE_INSTALL_FULL_INCLUDEDIR}
Requires: QtGui >= 4.8.0
