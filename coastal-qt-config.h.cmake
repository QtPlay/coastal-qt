/* Copyright (C) 2011 David Sugar, Tycho Softworks

   This file is free software; as a special exception the author gives
   unlimited permission to copy and/or distribute it, with or without
   modifications, as long as this notice is preserved.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
   implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#define PACKAGE "${PROJECT_NAME}"
#define VERSION "${VERSION}"
#define TRANSLATIONS "${CMAKE_INSTALL_FULL_DATADIR}/coastal-qt"
#cmakedefine    HAVE_WINDOWS_H  1
#cmakedefine	HAVE_UNISTD_H	1
#cmakedefine    HAVE_LIBXSS		1
#cmakedefine    HAVE_MAGIC		1
#cmakedefine    RUNTIME_MAGIC   "${RUNTIME_MAGIC}"

#ifdef  HAVE_WINDOWS_H
#ifdef _MSC_VER
#define NOMINMAX
#endif
#include <windows.h>
#endif
