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

    QListWidget *view = (QListWidget*)parent();
    ChatItem *item = (ChatItem *)view->item(index.row());
    if(item->series != Main::getSeries()) {
        int l, t, r, b;

        view->getContentsMargins(&l, &t, &r, &b);

        QRect ta = option.rect.adjusted(50, 120, 0, 0);
        ta = option.fontMetrics.boundingRect(ta, Qt::AlignLeft|Qt::TextWordWrap, item->text, 0);

        QString id = index.data(Qt::DisplayRole).toString();
        if(option.fontMetrics.width(id) >= 46) {
            while(option.fontMetrics.width(id + "...") > 46)
                id.chop(1);
            id += "...";
        }    
        item->user = id;
        item->size = QSize(r - l + 1, ta.height() + 4);
        item->series = Main::getSeries();
    }
    return item->size;
}

void ChatDisplay::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QListWidget *view = (QListWidget*)parent();
    ChatItem *item = (ChatItem *)view->item(index.row());

    if(option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
    }

    QRect r = option.rect.adjusted(0, 0, 0, 0);
    painter->drawText(r.left(), r.top(), 46, r.height(), Qt::AlignLeft|Qt::TextWordWrap, item->user, &r);

    r = option.rect.adjusted(50, 0, 0, 0);
    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft|Qt::TextWordWrap, item->text, &r);
}

ChatItem::ChatItem(const QString& id, const QString &msg, item_t t) :
QListWidgetItem(id)
{
    user = id;
    text = msg;
    type = t;
    series = Main::getSeries() - 1;
}

UserItem::UserItem(const QString& id) :
QListWidgetItem(id)
{
    active = update = 0;
}
