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
#include "coastal.h"
#include "ui_about.h"

CoastalMain::CoastalMain(bool tray) :
QMainWindow(NULL)
{   
    Coastal::bind();

    program_version = VERSION;
    program_about = program_name = "Coastal Application";
    program_copyright = "2011 David Sugar";

    trayicon = NULL;
    traymenu = dockmenu = _appmenu = NULL;
    url_support = "https://github.com/dyfet/coastal-qt/issues";

#if defined(Q_OS_WIN)
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QIcon::setThemeName("coastal");    
    setIconSize(QSize(24, 24));         // uniform icon size...
#elif defined(Q_OS_MAC)
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    QIcon::setThemeName("coastal");
#else
    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    if(!QIcon::hasThemeIcon("view-refresh")) {
        QIcon::setThemeName("coastal");
        setIconSize(QSize(24, 24));
    }
#endif

#if defined(Q_OS_MAC)
    dockmenu = new QMenu();
    qt_mac_set_dock_menu(dockmenu);
#endif

    if(!tray)
        return;

    trayicon = new QSystemTrayIcon(this);
    if(trayicon)
        traymenu = new QMenu();
}

QWidget *CoastalMain::extendToolbar(QToolBar *bar, QMenuBar *menu)
{
    CoastalToolbarHelper *tbh = toolbar_helpers.value(bar, NULL);
    if(!tbh) {
        tbh = new CoastalToolbarHelper(bar, this, menu);
        toolbar_helpers[bar] = tbh;
    }
    return tbh;
}

CoastalToolbarHelper::CoastalToolbarHelper(QToolBar *tb, QMainWindow *mp, QMenuBar *mb) :
QWidget(NULL)
{
    t = tb;
    m = mb;
    moving = false;
    window = mp;
    tb->addWidget(this);
    tb->installEventFilter(this);
#ifdef Q_OS_WIN
    tb->setStyleSheet("QToolButton:hover {background: #60a0ff;} QToolBar {background: #60A0ff; color: white; border: #60a0ff;}");
    if(mb)
        mb->setStyleSheet("QMenuBar { border-bottom: 0px;}");
#endif
}

bool CoastalToolbarHelper::eventFilter(QObject *object, QEvent *event)
{
    QMouseEvent *mouse = static_cast<QMouseEvent *>(event);
    QPoint pos;

    switch(event->type()) {
    case QEvent::MouseMove:
        if(!moving)
            break;

        pos = window->pos();
        pos.rx() += mouse->globalPos().x() - mpos.x();
        pos.ry() += mouse->globalPos().y() - mpos.y();
        window->move(pos);
        mpos = mouse->globalPos();
        event->accept();
        return true;
    case QEvent::MouseButtonPress:
        moving = false;
#if defined(Q_OS_WIN)
        if(m && m->isVisible())
            break;
#endif
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
        if(mouse->button() != Qt::LeftButton)
            break;

        moving = true;
        mpos = mouse->globalPos();
        event->accept();
        return true;
#else
        break;      // use native window manager on linux, bsd, etc...
#endif
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseTrackingChange:
        if(!moving)
            break;

        moving = false;
        event->accept();
        return true;
    default:
        break;
    }
    return QObject::eventFilter(object, event);
}

QMenu *appmenu(const char *id)
{
#if defined(Q_OS_MAC)
    if(!_appmenu) {
        _appmenu = QMenuBar(0);

        return _appmenu->addMenu(id);
    }
    return NULL;
#else
    return NULL;
#endif
}

bool CoastalMain::notify(const QString& title, const QString& body, QSystemTrayIcon::MessageIcon iconcode, int timeout)
{
    QString icon = "info";

    switch(iconcode)
    {
    case QSystemTrayIcon::Critical:
        icon = "error";
        break;
    case QSystemTrayIcon::Warning:
        icon = "warn";
        break;
    case QSystemTrayIcon::NoIcon:
        icon = "";
    case QSystemTrayIcon::Information:
        break;
    }

    if(Coastal::notify(title, body, icon))
        return true;

    if(!trayicon)
        return false;

    if(!trayicon->supportsMessages())
        return false;

    trayicon->showMessage(title, body, iconcode, timeout);
    return true;
}

void CoastalMain::support(void)
{
    Coastal::browser(url_support);
}

void CoastalMain::about(void)
{
    QString title = tr("About ") + program_name;
    CoastalAbout info(this);
    info.setWindowTitle(title);
    info.setVersion(tr("Version: ") + program_version);
    info.setAbout(tr(program_about));
    info.setCopyright(tr("Copyright (c) ") + program_copyright);
    QIcon icon = windowIcon();
    QPixmap image = icon.pixmap(48, 48, QIcon::Normal, QIcon::On);
    QGraphicsScene scene;
    QGraphicsPixmapItem *item = scene.addPixmap(image);
    item->setVisible(true);
    info.setImage(scene);
    info.exec();
}

