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

#include <coastal.h>

class Index;

class Main : public CoastalMain
{
Q_OBJECT

public:
    QStringList history;
    QDir dir;
    Index *ind;

    static bool casefilter;
    static bool bodyfilter;

    Main(const char *prefix);
    virtual ~Main();

public slots:
    void changeDir(void);
    void selectDir(int index);
    void clear(void);
    void reload(void);
    void open(void);
    void view(void);
    void open(const QModelIndex& index);
    void close(int tab);
    void menu(const QPoint& pos);
    void open(const QPoint& pos);
    void options(void);
};

