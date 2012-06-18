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
#include <cstring>
#include <cstdio>
#include <cctype>

View::View(QTabWidget *tabs, QString& title) :
QTextEdit()
{
    QString text;
    QFile file(title);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while(!file.atEnd()) {
            text += file.readLine();
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