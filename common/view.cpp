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

#include <coastal-qt-config.h>
#include <coastal.h>
#include "ui_find.h"
#include <cstring>
#include <cstdio>
#include <cctype>

CoastalView::CoastalView(QWidget *parent) :
QTextEdit(parent)
{
    seeking = "";
    findby = 0;
    setReadOnly(true);
}

void CoastalView::keyPressEvent(QKeyEvent *e)
{
    switch(e->nativeVirtualKey()) {
    case 65472:
        if(!seeking.isEmpty()) {
            search();
            break;
        }
    case 47:
    case 63:
    case 102:
    case 115:
    case 70:
    case 83:
        new CoastalFind(this);
    default:
        break;
    }
    QTextEdit::keyPressEvent(e);
}

void CoastalView::search()
{
    if(!seeking.isEmpty()) {
        if(!QTextEdit::find(seeking, findby))
            seeking = "";
    }
}

bool CoastalView::isSearching(void) const
{
    return !seeking.isEmpty();
}

CoastalFind::CoastalFind(CoastalView *view) :
QDialog(view)
{
    QString text;

    Ui::FindDialog ui;

    ui.setupUi((QDialog *)this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Find Text"));

    edit = ui.seekEdit;

    if(!view->seeking.isEmpty())
        edit->setText(view->seeking);

    connect(ui.nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui.prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    connect(ui.seekEdit, SIGNAL(returnPressed()), this, SLOT(enter()));

    show();
}

void CoastalFind::enter(void)
{
    CoastalView *view = (CoastalView *)parent();

    if(edit->text().isEmpty()) {
        view->seeking = "";
        emit close();
        return;
    }
    next();
    emit close();
}

void CoastalFind::next(void)
{
    CoastalView *view = (CoastalView *)parent();
    view->findby = 0;
    QString seeking = edit->text();

    if(seeking.isEmpty()) {
        view->seeking = "";
        emit close();
        return;
    }

    if(view->seeking.isEmpty()) {
        QTextCursor cursor(view->textCursor());
        cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
        view->setTextCursor(cursor);
    }
    view->seeking = seeking;
    view->search();
}

void CoastalFind::prev(void)
{
    CoastalView *view = (CoastalView *)parent();
    view->findby = QTextDocument::FindBackward;
    QString seeking = edit->text();

    if(seeking.isEmpty()) {
        view->seeking = "";
        emit close();
        return;
    }

    if(view->seeking.isEmpty()) {
        QTextCursor cursor(view->textCursor());
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        view->setTextCursor(cursor);
    }
    view->seeking = seeking;
    view->search();
}

