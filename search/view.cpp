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
    char buf[512];
    FILE *fp;

    fp = fopen(title.toUtf8().constData(), "r");
    while(fp) {
        if(feof(fp))
            break;
        if(!fgets(buf, sizeof(buf), fp))
            continue;
        char *cp = strchr(buf, '\n');
        if(cp) {
            *(cp--) = 0;
            if(cp >= buf && *cp == '\r')
                *cp = 0;
        }
        text = text + buf + "\n";
    }
    if(fp)
        fclose(fp);

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
