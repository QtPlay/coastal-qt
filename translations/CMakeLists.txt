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

QT4_CREATE_TRANSLATION(qm_search ${ts_search} coastal-search.ts)
QT4_CREATE_TRANSLATION(qm_manpager ${ts_manpager} coastal-manpager.ts)

add_custom_target(translate DEPENDS ${qm_search} ${qm_manpager})
