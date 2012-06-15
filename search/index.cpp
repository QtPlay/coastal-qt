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

Index::Index(QObject *parent, QString basename, QStringList ext) :
QAbstractTableModel(parent)
{
    rows = 0;

    if(basename.right(1) == "$")
        basename.chop(1);
    else if(basename.right(1) != "*")
        basename += "*";

    for(unsigned pos = 0; pos < (unsigned)ext.size(); ++pos)
        filters << (basename + ext[pos]);

    if(!(unsigned)ext.size()) {
        if(basename.isEmpty())
            filters << "*";
        else
            filters << basename;
    }

    scan("");
}

void Index::scan(QString path)
{
    QString spec;
    if(path.isEmpty())
        spec = QDir::currentPath();
    else
        spec = QDir::currentPath() + "/" + path;

    QDir dir(spec);
    if(!dir.exists())
        return;

    QStringList dirs = dir.entryList(QDir::AllDirs, QDir::SortFlags(QDir::Name | QDir::IgnoreCase));
    dir.setNameFilters(filters);
    QStringList list = dir.entryList(QDir::Files, QDir::SortFlags(QDir::Name | QDir::IgnoreCase));

    for(unsigned pos = 0; pos < (unsigned)dirs.size(); ++pos) {
        if(dirs[pos][0] == '.')
            continue;
        if(path.isEmpty())
            scan(dirs[pos]);
        else
            scan(path + "/" + dirs[pos]);
    }

    for(unsigned pos = 0; pos < (unsigned)list.size(); ++pos) {
        if(list[pos][0] == '.')
            continue;
        if(path.isEmpty())
            names << list[pos];
        else
            names << (path + "/" + list[pos]);
        ++rows;
    }
}

Index::~Index()
{
}

int Index::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return rows;
}

int Index::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant Index::data(const QModelIndex& index, int role) const
{
    int row = index.row();

    if(row >= rows || row < 0 || role != Qt::DisplayRole)
        return QVariant();

    return names[row];
}

QVariant Index::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QString Index::name(int row)
{
    return names[row];
}

