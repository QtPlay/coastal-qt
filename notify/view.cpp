// Copyright (C) 2011 David Sugar, Tycho Softworks`
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

#include "program.h"
#include "ui_main.h"

#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <time.h>

using namespace std;

ChatDisplay::ChatDisplay(QObject *parent) :
QStyledItemDelegate(parent)
{
}

QSize ChatDisplay::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    int l, t, r, b;
    QListWidget *view = (QListWidget*)parent();
    QString text = index.data(Qt::UserRole + 1).toString();

    view->getContentsMargins(&l, &t, &r, &b);

    QRect ta = option.rect.adjusted(50, 120, 0, 0);
    ta = option.fontMetrics.boundingRect(ta, Qt::AlignLeft|Qt::TextWordWrap, text, 0);

    return QSize(r - l + 1, ta.height() + 4);
}

void ChatDisplay::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QString id = index.data(Qt::DisplayRole).toString();
    QString t = index.data(Qt::UserRole + 1).toString();

    if(option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
    }

    QString out = id;
    if(option.fontMetrics.width(out) >= 46) {
        while(option.fontMetrics.width(out + "...") > 46)
            out.chop(1);
        out += "...";
    }    
    QRect r = option.rect.adjusted(0, 0, 0, 0);
    painter->drawText(r.left(), r.top(), 46, r.height(), Qt::AlignLeft|Qt::TextWordWrap, out, &r);

    r = option.rect.adjusted(50, 0, 0, 0);
    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft|Qt::TextWordWrap, t, &r);
}

UserItem::UserItem(const QString& id) :
QListWidgetItem(id)
{
    active = update = 0;
}

