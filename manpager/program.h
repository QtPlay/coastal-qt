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

#include "../config.h"
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
#include <QValidator>

#define COL_NAME    1
#define COL_SECTION 0

class ValidateIndex : public QValidator
{
public:
    ValidateIndex();

    virtual State validate(QString& input, int& pos) const;
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

private:
    QStringList names, sections;
    QList<Index::fileinfo> infos;
    bool triggered;
    unsigned rows, first, last;
    unsigned *map;

protected:
    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public:
    Index(QObject *parent = NULL);
    virtual ~Index();

    QString name(int row);
    fileinfo node(int row);

    int search(const QString& name);
    int find(const QString& name);
    void select(int pos, const QString& name);

signals:
    void selected(const QString& text);
};

class View : public QTextEdit
{
Q_OBJECT

public:
    View(QTabWidget *tabs, QIODevice& source, QString& title);

    static bool find(QTabWidget *tabs, QString& title);
};

class Main : public CoastalMain
{
Q_OBJECT

private:
    Index *indexData;
    ValidateIndex index_validator;

    void load(int row);

public:
    static QStringList manpaths;
    static bool hidden[10];

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
    void load(void);
    void close(int tab);
    void columns(void);
    void search(const QString& text);
    void menu(const QPoint& pos);
    void open(const QPoint& pos);
    void open(void);
};

#endif

