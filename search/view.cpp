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

#include "program.h"
#include "ui_main.h"
#include "ui_find.h"
#include <cstring>
#include <cstdio>
#include <cctype>

View::View(QTabWidget *tabs, QString& title) :
QTextEdit()
{
    seeking = "";
    findby = 0;

    QString text, temp;
    int pos;
    QFile file(title);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while(!file.atEnd()) {
            temp = file.readLine();
            while((pos = temp.indexOf("\002")) > -1) {
                temp.remove(pos, 1);
            }
            while((pos = temp.indexOf("\003")) > -1) {
                temp.remove(pos, 3);
            }
            while((pos = temp.indexOf("\010")) > -1) {
                temp.remove(pos, 1);
            }
            while((pos = temp.indexOf("\017")) > -1) {
                temp.remove(pos, 1);
            }
            text += temp;
        }
    }

    setReadOnly(true);
    setEnabled(true);
    setText(text);

    int views = tabs->count();

    tabs->addTab(this, title);
    tabs->setCurrentIndex(views);
    tabs->setTabsClosable(true);
}

void View::keyPressEvent(QKeyEvent *e)
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
        new Find(this);
    default:
        break;
    }
    QTextEdit::keyPressEvent(e);
}

void View::search()
{
    if(!seeking.isEmpty()) {
        if(!QTextEdit::find(seeking, findby))
            seeking = "";
    }
}

bool View::find(QTabWidget *tabs, QString& title)
{
    for(int tab = 1; tab < tabs->count(); ++tab) {
        if(tabs->tabText(tab) == title) {
            tabs->setCurrentIndex(tab);
            return true;
        }
    }
    return false;
}

Find::Find(View *view) :
QDialog(view)
{
    QString text;

    Ui::Find ui;

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

void Find::enter(void)
{
    View *view = (View *)parent();

    if(edit->text().isEmpty()) {
        view->seeking = "";
        emit close();
        return;
    }
    next();
    emit close();
}

void Find::next(void)
{
    View *view = (View *)parent();
    view->findby = 0;
    QString seeking = edit->text();

    if(seeking.isEmpty()) {
        view->seeking = "";
        emit close();
        return;
    }

    if(Main::caseflag)
        view->findby |= QTextDocument::FindCaseSensitively;

    if(view->seeking.isEmpty()) {
        QTextCursor cursor(view->textCursor());
        cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
        view->setTextCursor(cursor);
    }
    view->seeking = seeking;
    view->search();
}

void Find::prev(void)
{
    View *view = (View *)parent();
    view->findby = QTextDocument::FindBackward;
    QString seeking = edit->text();

    if(seeking.isEmpty()) {
        view->seeking = "";
        emit close();
        return;
    }

    if(Main::caseflag)
        view->findby |= QTextDocument::FindCaseSensitively;

    if(view->seeking.isEmpty()) {
        QTextCursor cursor(view->textCursor());
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        view->setTextCursor(cursor);
    }
    view->seeking = seeking;
    view->search();
}

