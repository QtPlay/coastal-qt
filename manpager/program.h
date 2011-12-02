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

#ifndef MAIN_H_
#define MAIN_H_

#include <config.h>
#include <coastal.h>
#include <QWidget>
#include <QObject>
#include <QApplication>
#include <QMainWindow>
#include <QTranslator>
#include <QSettings>
#include <QAction>
#include <QDir>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QProcess>
#include <QTextEdit>
#include <QHeaderView>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#define COL_NAME    1
#define COL_SECTION 0

class Main : public CoastalMain
{
Q_OBJECT

public:
    QStringList manpaths;
    QActionGroup *searchGroup, *selectGroup;

    Main(QWidget *parent = NULL);
    virtual ~Main();

    void status(const QString& text);
    void error(const QString& text);

    void resizeEvent(QResizeEvent *e);

signals:
    void startup(void);
    void resized(void);

public slots:
    void reload(void);
    void load(const QModelIndex& index);
    void close(int tab);
    void columns(void);
};

class Index : public QAbstractTableModel
{
    Q_OBJECT

public:
    typedef struct {
        enum {NORMAL, GZIP} mode;
        unsigned path;
        char id;
    }   fileinfo;

    Index(QObject *parent = NULL);
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    static QString name(int row);
    static fileinfo node(int row);
    static void clear(QTableView *view);
    static void set(QTableView *view);
    static void add(QDir& dir, char group, unsigned path);
};

class View : public QTextEdit
{
Q_OBJECT

public:
    View(QTabWidget *tabs, QIODevice& source, QString& title);

    static bool find(QTabWidget *tabs, QString& title);
};
#endif

