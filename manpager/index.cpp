// Copyright (C) 2011-2015 David Sugar, Tycho Softworks
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
#include "index.h"
#include "main.h"

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
            QDir dir(Main::manpaths[path] + QDir::separator() + mandirs[section]);
            if(!dir.exists())
                continue;
            if(Main::hidden[section])
                continue;

            info.path = path;
            info.mode = fileinfo::NORMAL;
            info.id = cmap[section];

            QStringList list = dir.entryList(QDir::Files);
            for(unsigned pos = 0; pos < (unsigned)list.size(); ++pos) {
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

                byname[entry] = rows;
                ++rows;
            }
        }
    }

    if(!rows)
        return;

    map = new unsigned[rows];

    foreach(unsigned value, byname)
        map[last++] = value;

    rows = last;
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

    if(row >= (int)(last - first) || row < 0 || role != Qt::DisplayRole)
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

QString Index::nameAt(int row) const
{
    return names[map[row + first]] + "." + sections[map[row + first]];
}

Index::fileinfo Index::nodeAt(int row) const
{
    return infos[map[row + first]];
}

void Index::selectAt(int pos, const QString& name)
{
    int len = name.length();

    if(len < 1) {
        first = 0;
        last = rows;
        return;
    }

    first = last = pos;
    while(last < rows && name == findFrom(last, len))
        ++last;

    // signal input combobox if single entry found

    if(last != first + 1) {
        triggered = false;
        return;
    }

    if(triggered)
        return;

    emit selected(nameFrom(first));
    triggered = true;
}

int Index::search(const QString& name) const
{
    unsigned diff = (last - first) / 2;
    unsigned pos = first + diff;
    QString check = nameFrom(pos);
    unsigned len = name.length();

    while(diff > 0) {
        diff /= 2;

        if(name == check.left(len))
            return pos - first;

        if(name < check) {
            if(diff)
                pos -= diff;
            else
                --pos;
        }
        else {
            if(diff)
                pos += diff;
            else
                ++pos;
        }
        if(pos < first)
            pos = first;
        if(pos >= last)
            pos = last - 1;

        check = nameFrom(pos);
    }

    while(name < check.left(len) && pos > first) {
        --pos;
        check = nameFrom(pos);
    }

    while(name > check.left(len) && pos < (last - 1)) {
        ++pos;
        check = nameFrom(pos);
    }

    if(name != check.left(len))
        return -1;

    return pos - first;
}


int Index::find(const QString& name) const
{
    unsigned diff = rows / 2;
    unsigned pos = diff;
    unsigned len = name.length();
    if(rows < 1)
        return -1;

    QString check = findFrom(pos, len);

    while(diff > 0) {
        diff /= 2;

        if(name == check)
            break;

        if(name < check) {
            if(diff)
                pos -= diff;
            else
                --pos;
        }
        else {
            if(diff)
                pos += diff;
            else
                ++pos;
        }

        if(pos >= rows)
            pos = rows - 1;

        check = findFrom(pos, len);
    }

    while(name < check && pos > 0)
        check = findFrom(--pos, len);

    while(name > check && pos < rows - 1)
        check = findFrom(++pos, len);

    if(name != findFrom(pos, len))
        return -1;

    while(pos > 0 && name == findFrom(pos - 1, len))
        --pos;

    return pos;
}

