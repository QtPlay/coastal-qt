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

class Main : public CoastalMain
{
Q_OBJECT

public:
    QStringList manpaths;

    Main(QWidget *parent = NULL);
    virtual ~Main();

    void status(const QString& text);

public slots:
    void reload(void);
    void scan(void);
};

class Index
{
public:
    class NameItem : public QTableWidgetItem
    {
    public:
        QString path;

        NameItem(QString& name, QString& fullpath);
    };

    class SectionItem : public QTableWidgetItem
    {
    public:
        char id;

        SectionItem(QString& section, char group);
    };

    static void set(QTableWidget *table);
    static void add(QDir& dir, char group);
};

#endif

