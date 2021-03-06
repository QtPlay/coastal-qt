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
#include <ui_about.h>

CoastalBusy::CoastalBusy()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    qApp->installEventFilter(this);
}

CoastalBusy::~CoastalBusy()
{
    qApp->processEvents();
    qApp->removeEventFilter(this);
    QApplication::restoreOverrideCursor();
}

bool CoastalBusy::eventFilter(QObject *obj, QEvent *evt)
{
    switch(evt->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
	return true;
    default:
	return QObject::eventFilter(obj, evt);
    }
}

