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
#include "view.h"

View::View(QTabWidget *tabs, QString& title) :
CoastalView()
{
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

