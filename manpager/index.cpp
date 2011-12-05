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

static const char *cmap = "12345678ln";
static QStringList mandirs = QStringList() << "man1" << "man2" << "man3" << "man4" << "man5" << "man6" << "man7" << "man8" << "manl" << "mann";

Index::Index(QObject *parent) :
QAbstractTableModel(parent)
{
    int ext;
    Index::fileinfo info;
    QMap<QString, unsigned> byname;

    rows = first = last = 0;
    map = NULL;

    for(int section = 0; section < 10; ++section) {
        Main::status(tr("loading ") + cmap[section] + "...");
        for(int path = 0; path < Main::manpaths.size(); ++path) {
            QDir dir(Main::manpaths[path] + "/" + mandirs[section]);
            if(!dir.exists())
                continue;
            if(Main::hidden[section])
                continue;

            info.path = path;
            info.mode = fileinfo::NORMAL;
            info.id = cmap[section];

            QStringList list = dir.entryList(QDir::Files);
            for(unsigned pos = 0; pos < list.size(); ++pos) {
                QString entry = list[pos];
                info.mode = fileinfo::GZIP;
                ext = entry.lastIndexOf('.');
                if(ext < 2)
                    continue;

                if(entry.mid(ext) == ".gz") {
                    entry = entry.left(ext);
                    ext = entry.lastIndexOf('.');
                    if(ext < 2)
                        continue;
                }

                names << entry.left(ext);
                sections << entry.mid(++ext);
                infos << info;

                byname.insert(entry, rows);
                ++rows;
            }
        }
    }

    if(!rows)
        return;

    map = new unsigned[rows];

    foreach(unsigned value, byname)
        map[last++] = value;
}

Index::~Index()
{
    if(map)
        delete[] map;
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

    if(row >= (last - first) || row < 0 || role != Qt::DisplayRole)
        return QVariant();

    switch(index.column()) {
    case 0:
        return names[map[row + first]];
    case 1:
        return sections[map[row + first]];
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
    return names[map[row + first]] + "." + sections[map[row + first]];
}

Index::fileinfo Index::node(int row)
{
    return infos[map[row + first]];
}

