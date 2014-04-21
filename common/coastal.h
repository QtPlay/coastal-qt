// Copyright (C) 2011-2013 David Sugar, Tycho Softworks
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

#ifndef _COASTAL_H_
#define _COASTAL_H_

#include <QtCore>
#include <QWidget>
#include <QtGui>
#include <QLineEdit>
#include <QGraphicsScene>
#include <QSystemTrayIcon>
#include <QDialog>
#include <QMainWindow>
#include <QTextEdit>
#include <QGraphicsPixmapItem>
#include <QActionGroup>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QCheckBox>
#include <QListWidgetItem>
#include <QStyledItemDelegate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

class CoastalAbout : public QDialog
{
Q_OBJECT

public:
    CoastalAbout(QWidget *parent = NULL);

    void setImage(QGraphicsScene& scene);
    void setVersion(const QString& text);
    void setAbout(const QString& text);
    void setCopyright(const QString& text);
};

class CoastalNotify : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        None,
        DBUS
    } type_t;

    typedef enum {
        Expired     = 1,
        Dismissed   = 2,
        ForceClosed = 3,
        Unknown     = 4
    } reason_t;

    typedef enum
    {
        Low         = 0,
        Normal      = 1,
        High        = 2
    } priority_t;

    CoastalNotify(const QString& title, const QString& body, const QString& icon = "info", QObject* parent = NULL);
    ~CoastalNotify();

    bool update(void);
    void release(void);

    void setTitle(const QString& title);
    void setBody(const QString& body);
    void setIcon(const QString& icon);
    void setTimeout(int timeout);

    bool setActions(const QStringList& actions, int defaction = -1);
    bool setPriority(priority_t priority);

    static type_t getType(void);

private slots:
    void dbus_action(uint id, QString key);
    void dbus_close(uint, uint);

signals:
    void closed(reason_t reason);
    void activated(int action);

private:
    void *_dbus;
    uint _id;
    bool _updated;

    QString _title;
    QString _body;
    QString _icon;
    QStringList _actions;
    QVariantMap _hints;
    int _action;
    int _timeout;
};

class CoastalMain : public QMainWindow
{
Q_OBJECT

private:
    QMenu *_appmenu;

protected:
    const char *program_version, *program_about, *program_copyright, *program_name;
    const char *url_support;
    QSystemTrayIcon *trayicon;
    QMenu *traymenu;

    CoastalMain(bool tray = false);

    QMenu *appmenu(const char *id);

public:
    bool notify(const QString& title, const QString& body, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int timeout = 10000);

public slots:
    void about(void);
    void support(void);
};

class CoastalDialog : public QDialog
{
Q_OBJECT

protected:
    const char *dialog_version, *dialog_about, *dialog_copyright, *dialog_name;
    const char *url_support;

    QSystemTrayIcon *trayicon;
    QMenu *traymenu;

    CoastalDialog(bool tray = false);

public:
    bool notify(const QString& title, const QString& body, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int timeout = 10000);

public slots:
    void about(void);
    void support(void);
};

class CoastalView : public QTextEdit
{
private:
    friend class CoastalFind;

    void keyPressEvent(QKeyEvent *event);

    QTextDocument::FindFlags findby;
    QString seeking;

public:
    CoastalView(QWidget *parent = NULL);

    void search(void);

    bool isSearching(void) const;

    static bool sensitive(void);

    static void setSensitive(bool enable = true);
};

class CoastalFind : public QDialog
{
Q_OBJECT

private:
    QLineEdit *edit;

public:
    CoastalFind(CoastalView *view);

public slots:
    void next(void);
    void prev(void);
    void enter(void);
};

class CoastalBusy : public QObject
{
private:
    bool eventFilter(QObject *obj, QEvent *event);

public:
    CoastalBusy();
    ~CoastalBusy();
};

class Coastal
{
public:
    static bool env(const char *id, char *buffer, size_t size);

    static void icons(const char *check_id = "view-refresh");

    static QString userid(void);

    static QString env(const QString& id);

    static QString extension(const QString& filename);

    static QString mimefile(const QString& filename);

    static QString mimetype(const QString& extension);

    static bool browser(const QString& url);

    static bool open(const QString& filename);

    static bool notify(const QString& title, const QString& body, const QString& icon = "info");

    static bool away(void);

    static bool email(QString& title, QString& body);

    static bool emailto(QString& to, QString& title, QString& body);

};

#endif

