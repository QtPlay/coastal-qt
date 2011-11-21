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

static unsigned views = 0;

View::View(QTabWidget *tabs, QIODevice& input, QString& title) :
QTextEdit()
{
    char buf[1024];
    qint64 len;

    for(;;) {
        len = input.readLine(buf, (qint64)sizeof(buf));
        qDebug() << "LEN " << len << " text " << buf;
        if(len < 1)
            break;
    }
    setReadOnly(true);
    setEnabled(true);

    tabs->addTab(this, title);
    tabs->setCurrentIndex(++views);
    tabs->setTabsClosable(true);
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
