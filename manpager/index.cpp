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

static QTableWidget *table;
static int line = 0;

Index::NameItem::NameItem(QString& name, QString& dirpath)
{
    path = dirpath;
    setText(name);
    setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    table->setItem(line, 1, (QTableWidgetItem *)this);
}

Index::SectionItem::SectionItem(QString& section, char group)
{
    id = group;
    setText(section);
    setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    table->setItem(line, 0, (QTableWidgetItem *)this);
}

void Index::set(QTableWidget *t)
{
    table = t;
    table->clearContents();
    table->setSortingEnabled(true);
    line = 0;
}

void Index::add(QDir& dir, char group)
{
    int last;
    QStringList list = dir.entryList(QDir::Files);
    for(unsigned pos = 0; pos < list.size(); ++pos) {
        QString entry = list[pos];
        QString path = dir.path() + "/" + entry;

        last = entry.lastIndexOf('.');
        if(last < 2)
            continue;

        if(entry.mid(last) == ".gz") {
            entry = entry.left(last);
            last = entry.lastIndexOf('.');
            if(last < 2)
                continue;
        }

        QString name = entry.left(last);
        QString section = entry.mid(++last);
        if(section[0] != QChar(group))
            continue;

        table->insertRow(line);
        new SectionItem(section, group);
        new NameItem(name, path);
        ++line;
    }
    table->sortByColumn(1, Qt::AscendingOrder);
    table->update();
}

