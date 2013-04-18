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

class Options : private QSettings
{
Q_OBJECT

public:
    QString network;
    qreal opacity;
    unsigned timeout;
    bool show_icons;

    Options();

    void save(void);

public slots:
    void change(QString net);
};

class Fifo : public QThread
{
Q_OBJECT

private:
    void process(char *buf);

public:
    Fifo();

    void stop(void);

    void run(void);

signals:
    void notice(QString title, QString summary, QString icon);
    void restart(void);
};

class Main : public CoastalDialog
{
Q_OBJECT

private:
    Fifo *fifo;

public:
    static bool restart_flag;

    Main();
    virtual ~Main();
    
    void stop(void);

    Options options;

public slots:
    void action(QSystemTrayIcon::ActivationReason reason);

    void notice(QString title, QString summary, QString icon = "");

    void restart(void);
};

#ifndef QT_DBUS_LIB

class Notice : public QDialog
{
Q_OBJECT

private:
    QTimer timer;

public:
    Notice(Options& options, QString& title, QString& summary, QString& icon);
    ~Notice();

    static unsigned count(void);
};

#endif

#endif

