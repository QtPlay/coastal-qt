// Copyright (C) 2011 David Sugar, Tycho Softworks
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

#include "program.h"

Index::Index(QObject *parent) :
QAbstractTableModel(parent)
{
    rows = 0;
}

int Index::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return rows;
}

int Index::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant Index::data(const QModelIndex& index, int role) const
{
    int row = index.row();

    if(row >= rows || row < 0 || role != Qt::DisplayRole)
        return QVariant();

    switch(index.column()) {
    case 0:
        return names[row];
    case 1:
        return sections[row];
    default:
        return QVariant();
    }
}

QVariant Index::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    switch(section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Section");
    default:
        return QVariant();
    }
}

QString Index::name(int row)
{
    return names[row] + "." + sections[row];
}

Index::fileinfo Index::node(int row)
{
    return infos[row];
}

void Index::add(QDir& dir, char group, unsigned path)
{
    int last;
    Index::fileinfo info;

    info.path = path;
    info.mode = fileinfo::NORMAL;
    info.id = group;

    QStringList list = dir.entryList(QDir::Files);
    for(unsigned pos = 0; pos < list.size(); ++pos) {
        QString entry = list[pos];
        info.mode = fileinfo::GZIP;
        last = entry.lastIndexOf('.');
        if(last < 2)
            continue;

        if(entry.mid(last) == ".gz") {
            entry = entry.left(last);
            last = entry.lastIndexOf('.');
            if(last < 2)
                continue;
        }

        names << entry.left(last);
        sections << entry.mid(++last);
        infos << info;
        ++rows;
    }
}

