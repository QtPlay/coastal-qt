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
#include "ui_notice.h"

using namespace std;

volatile unsigned instances = 0;

Notice::Notice(Options& options, const char *title, const char *summary, const char *type) : QDialog(NULL)
{
    QString text;

    Ui::NoticeDialog ui;

    ui.setupUi((QDialog *)this);

    text = title;
    if(summary) {
        text += "\n";
        text += summary;
    }

    ui.Text->setText(text);

    setAttribute(Qt::WA_DeleteOnClose);
    if(type && (!strcmp(type, "error") || !strcmp(type, "dialog-error")))
        setWindowIcon(QIcon(":/error.png"));
    else if(type && (!strcmp(type, "warning") || !strcmp(type, "dialog-warning"))) 
        setWindowIcon(QIcon(":/warning.png"));
    else
        setWindowIcon(QIcon(":/info.png"));

    if(options.show_icons)
        ui.Icon->setIcon(windowIcon());
    else {
        ui.Icon->setVisible(false);
        ui.Icon->setEnabled(false);
        ui.Icon->hide();
    }

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowOpacity(options.opacity);

    ++instances;

    connect(&timer, SIGNAL(timeout()), this, SLOT(close()));
    timer.start(options.timeout);
    show();
}

Notice::~Notice()
{
    --instances;
}

unsigned Notice::count(void)
{
    return instances;
}














