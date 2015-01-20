// Copyright (C) 2011-2013 David Sugar, Tycho Softworks
//
// This file is part of coastal-qt.
//
// Coastal-qt is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// coastal-qt is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with coastal-qt.  If not, see <http://www.gnu.org/licenses/>.

#include <coastal-qt-config.h>
#include "coastal.h"

CoastalMapped::CoastalMapped(QString filepath) :
QFile(filepath)
{
    mapsize = 0;
    mapaddr = NULL;

    if(!open(QFile::ReadOnly))
        return;

    mapsize = QFile::size();
    mapaddr = QFile::map((qint64)0, (qint64)mapsize);
    if(!mapaddr)
        mapsize = 0;
    QFile::close();
}

CoastalMapped::~CoastalMapped()
{
    release();
}

bool CoastalMapped::copy(size_t offset, void *buffer, size_t bufsize) const
{
    if(!mapaddr || (offset + bufsize > mapsize)) {
        fault();
        return false;
    }

    const void *member = (const void *)(mapaddr + offset);

    do {
        memcpy(buffer, member, bufsize);
    } while(memcmp(buffer, member, bufsize));

    return true;
}

void CoastalMapped::release(void)
{
    if(mapaddr) {
        QFile::unmap(mapaddr);
        mapaddr = NULL;
        mapsize = 0;
    }
}

void CoastalMapped::fault(void) const
{
    abort();
}












