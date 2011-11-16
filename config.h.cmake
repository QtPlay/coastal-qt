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
#define TRANSLATIONS "${DEFAULT_TRANSLATIONS}"
#cmakedefine    HAVE_WINDOWS_H  1

#ifdef  HAVE_WINDOWS_H
#include <windows.h>
#endif
