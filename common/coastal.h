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
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>

/**
 * @brief Create a coastal styled about box.
 * This is used to show basic application information.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class CoastalAbout : public QDialog
{
Q_OBJECT

public:
    /**
     * @brief Create coastal about window.
     * @param parent window
     */
    CoastalAbout(QWidget *parent = NULL);

    /**
     * @brief set the graphic splash to show.
     * @param scene
     */
    void setImage(QGraphicsScene& scene);

    /**
     * @brief setVersion
     * @param text of application version to present.
     */
    void setVersion(const QString& text);

    /**
     * @brief set text we present about the application.
     * @param text to show
     */
    void setAbout(const QString& text);

    /**
     * @brief set text of copyright message
     * @param text for copyright
     */
    void setCopyright(const QString& text);
};

/**
 * @brief Interface to generic OSD notifications
 * This provides an interface to desktop notification services.
 * These might include the freedesktop dbus notify service, or
 * other platform specific services.  This is used to augment
 * the Qt systray notification services in CoastalMain.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
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

/**
 * @brief Helper class for extending main window toolbars.
 * This helper allows one to move the application when the
 * toolbar is moved.  This is particularly helpful for OS/X
 * when the toolbar has been "blended" with the titlebar.
 * It also provides a place to attach widgets and layouts to
 * a toolbar.
 */
class CoastalToolbarHelper : public QWidget
{
Q_OBJECT

private:
    friend class CoastalMain;

    QPoint mpos;
    bool moving;
    QToolBar *t;
    QMenuBar *m;
    QMainWindow *window;

    CoastalToolbarHelper(QToolBar *tb, QMainWindow *mp, QMenuBar *mb = NULL);

    bool eventFilter(QObject *, QEvent *);
};

/**
 * @brief An augmented main window class.
 * This provides many enhancements over the generic QMainWindow
 * class, including setup of application toolbars, desktop icon
 * themes, expanded notification services, and system tray/dock
 * integration.  It also provides for integration of about splash
 * pages and access to support services.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class CoastalMain : public QMainWindow
{
Q_OBJECT

private:
    QMenu *_appmenu;
    QMap <QToolBar *, CoastalToolbarHelper *> toolbar_helpers;

protected:
    const char *program_version, *program_about, *program_copyright, *program_name;
    const char *url_support;
    QSystemTrayIcon *trayicon;
    QMenu *traymenu, *dockmenu;

    CoastalMain(bool tray = false);

    QMenu *appmenu(const char *id);

public:
    bool notify(const QString& title, const QString& body, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int timeout = 10000);

    QWidget *extendToolbar(QToolBar *bar, QMenuBar *menu = NULL);

public slots:
    void about(void);
    void support(void);
};

/**
 * @brief An augmented main dialog window class.
 * For applications that do not have a "main window", this provides
 * an enhanced dialog class to use as an application "main".  This
 * includes integration of system tray/dock functionality, integration
 * of system notification services, and some of the support also
 * found in CoastalMain.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class CoastalDialog : public QDialog
{
Q_OBJECT

protected:
    const char *dialog_version, *dialog_about, *dialog_copyright, *dialog_name;
    const char *url_support;

    QSystemTrayIcon *trayicon;
    QMenu *traymenu, *dockmenu;

    CoastalDialog(bool tray = false);

public:
    bool notify(const QString& title, const QString& body, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int timeout = 10000);

public slots:
    void about(void);
    void support(void);
};

/**
 * @brief Enhanced text view class.
 * Mostly this provides a text viewer that also has an integrated
 * text search capability.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class CoastalView : public QTextEdit
{
private:
    friend class CoastalFindHelper;

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

/**
 * @brief Helper class for coastal view.
 * This provides the view search/replace functionality.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class CoastalFindHelper : public QDialog
{
Q_OBJECT

private:
    friend class CoastalView;

    QLineEdit *edit;

    CoastalFindHelper(CoastalView *view);

private slots:
    void next(void);
    void prev(void);
    void enter(void);
};

/**
 * @brief Busy state for modal applications
 * This is used to set an application to "busy" while it
 * performs some long operation that may happen in the gui
 * thread context.  The application icon is set to busy.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class CoastalBusy : public QObject
{
private:
    bool eventFilter(QObject *obj, QEvent *event);

public:
    CoastalBusy();
    ~CoastalBusy();
};

/**
 * @brief A collection of coastal utility functions.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class Coastal
{
private:
    friend class CoastalMain;

    static bool internal_icons;

public:
    static bool env(const char *id, char *buffer, size_t size);

    static QString userid(void);

    static QString env(const QString& id);

    static QString extension(const QString& filename);

    static QString mimeFile(const QString& filename);

    static QString mimeType(const QString& extension);

    static bool browser(const QString& url);

    static bool open(const QString& filename);

    static bool notify(const QString& title, const QString& body, const QString& icon = "info");

    static bool away(void);

    static bool email(QString& title, QString& body);

    static bool emailTo(QString& to, QString& title, QString& body);

    static bool applyStyle(QApplication& app, QString style);

    static void paintRect(QPainter *painter, const QRect& rect, QColor color, qreal outline = 0.0, qreal xradius = 0.0, qreal yradius = 0.0);

    static void paintBadge(QPainter *painter, QPoint pos, unsigned size, unsigned radius, QString text, QColor badge_color, QColor text_color, QFont text_font = qApp->font());
};

#endif

