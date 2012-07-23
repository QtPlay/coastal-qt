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

#include "../config.h"
#include <coastal.h>

#ifdef  QT_DBUS_LIB
#include <dbus_notify.h>

CoastalNotify::CoastalNotify(const QString& title, const QString& body, const QString& icon, QObject *parent) :
QObject(parent)
{

    _id = 0;
    _title = title;
    _body = body;
    _icon = icon;
    _timeout = -1;
    _updated = true;

    OrgFreedesktopNotificationsInterface* dbus = new OrgFreedesktopNotificationsInterface("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus(), this);
    connect(dbus, SIGNAL(NotificationClosed(uint, uint)),
        this, SLOT(dbus_close(uint,uint)));
    connect(dbus, SIGNAL(ActionInvoked(uint,QString)),
        this, SLOT(dbus_action(uint,QString)));
    _dbus = (void *)dbus;
}

CoastalNotify::~CoastalNotify()
{
    OrgFreedesktopNotificationsInterface *dbus = (OrgFreedesktopNotificationsInterface *)_dbus;
    if(dbus != NULL) {
        delete dbus;
        _dbus = NULL;
    }
}

bool CoastalNotify::update(void)
{
    bool result = true;

    if(!_updated)
        return true;

    OrgFreedesktopNotificationsInterface *dbus = (OrgFreedesktopNotificationsInterface *)_dbus;

    QDBusPendingReply<uint> reply = dbus->Notify(qAppName(), _id, _icon, _title, _body, _actions, _hints, _timeout);
    reply.waitForFinished();
    _updated = false;
    if (reply.isError())
        result = false;
    _id = reply.value();
    return result;
}

void CoastalNotify::release(void)
{
    OrgFreedesktopNotificationsInterface *dbus = (OrgFreedesktopNotificationsInterface *)_dbus;

    dbus->CloseNotification(_id);
    _id = 0;
}

bool CoastalNotify::setActions(const QStringList& actions, int defaction)
{
    _actions.clear();
    _action = defaction;
    for (int i = 0; i < actions.size(); ++i) {
        if (i == _action)
            _actions.append("default");
        else
            _actions.append(QString::number(i));
        _actions.append(actions[i]);
    }
    _updated = true;
    return true;
}

bool CoastalNotify::setPriority(priority_t priority)
{
    _hints.insert("urgency", qvariant_cast<uchar>(priority));
    _updated = true;
    return true;
}

CoastalNotify::type_t CoastalNotify::getType(void)
{
    return DBUS;
}

#else

bool CoastalNotify::setActions(const QStringList actions, int defaction)
{
    return false;
}

bool CoastalNotify::setPriority(priority_t priority)
{
    return false;
}

CoastalNotify::type_t CoastalNotify::getType(void)
{
    return None;
}

#endif

void CoastalNotify::setTitle(const QString& title)
{
    _title = title;
    _updated = true;
}

void CoastalNotify::setBody(const QString& body)
{
    _body = body;
    _updated = true;
}

void CoastalNotify::setIcon(const QString& icon)
{
    _icon = icon;
    _updated = true;
}

void CoastalNotify::setTimeout(int timeout)
{
    _timeout = timeout;
    _updated = true;
}

void CoastalNotify::dbus_action(uint id, QString key)
{
    bool valid = true;
    int act;

    if (id != _id)
        return;

    if (key == "default")
        act = _action;
    else
        act = key.toInt(&valid);

    if (valid && act >= 0)
        emit activated(act);
}

void CoastalNotify::dbus_close(uint id, uint reason)
{
    if (id != 0 && id == _id) {
        _id = 0;
    }
    emit closed((reason_t)reason);
}
