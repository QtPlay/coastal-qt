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
#include <coastal.h>

#define COL_NAME    1
#define COL_SECTION 0

class Index;
class ValidateIndex;

class Main : public CoastalMain
{
Q_OBJECT

private:
    Index *indexData;
    ValidateIndex *index_validator;

    void load(int row);

public:
    static QStringList manpaths;
    static bool hidden[10];
    static QAction *sections[10];

    QActionGroup *searchGroup, *selectGroup;

    Main(QWidget *parent = NULL);
    virtual ~Main();

    static void status(const QString& text);
    static void error(const QString& text);

    void resizeEvent(QResizeEvent *e);

signals:
    void startup(void);
    void resized(void);

public slots:
    void all(void);
    void clear(void);
    void reload(void);
    void load(const QModelIndex& index);
    void load(const QString& text);
    void close(int tab);
    void columns(void);
    void search(const QString& text);
    void menu(const QPoint& pos);
    void open(const QPoint& pos);
    void view(void);
    void open(void);
    void options(void);
};


