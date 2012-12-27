prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_PREFIX}/lib
includedir=${CMAKE_INSTALL_PREFIX}/include

Name: libcoastal
Description: coastal library
Version: ${COASTAL_VERSION}
Libs: -lcoastal
CFlags:
Requires: QtGui >= 4.8.0
