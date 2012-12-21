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

#include <coastal-qt-config.h>
#include <coastal.h>
#include <QWidget>
#include <QObject>
#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QTranslator>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QFileDialog>
#include <QAbstractTableModel>
#include <QTextEdit>
#include <QKeyEvent>
#include <QDebug>

class Index : public QAbstractTableModel
{
    Q_OBJECT

private:
    QStringList filters;
    QStringList names;
    int rows;

protected:
    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public:
    Index(QObject *parent, QString basename, QStringList ext, QString match);
    virtual ~Index();

    void scan(QString path, QString match);
    bool grep(QString& path, QString& match);

    QString name(int row);
};

class Config : public QDialog
{
Q_OBJECT

private:
    Config(QTabWidget *tabs);

public:
    ~Config();
    static void create(QTabWidget *tabs);

public slots:
    void configCase(int state);
};

class View : public QTextEdit
{
Q_OBJECT

private:
    void keyPressEvent(QKeyEvent *event);

    QTextDocument::FindFlags findby;
    QString seek;

public:
    View(QTabWidget *tabs, QString& title);

    void search(void);

    static bool find(QTabWidget *tabs, QString& title);
};

class Main : public CoastalMain
{
Q_OBJECT

public:
    QStringList history;
    QDir dir;
    Index *ind;

    static Qt::CaseSensitivity caseflag;

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

#endif

