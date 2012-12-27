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
#include "ui_config.h"
#include <cstring>
#include <cstdio>
#include <cctype>

static int last = -1;
static Ui::Config ui;

Config::Config(QTabWidget *tabs) :
QDialog()
{
    ui.setupUi((QDialog *)this);

    if(Main::caseflag == Qt::CaseSensitive)
        ui.checkSensitive->setChecked(true);
    else
        ui.checkSensitive->setChecked(false);

    connect(ui.checkSensitive, SIGNAL(stateChanged(int)), this, SLOT(configCase(int)));

    int views = tabs->count();

    tabs->addTab(this, "Options");
    tabs->setCurrentIndex(views);
    tabs->setTabsClosable(true);
    last = views;
}

Config::~Config()
{
    last = -1;
}

void Config::create(QTabWidget *tabs)
{
    if(last > -1)
        tabs->setCurrentIndex(last);
    else
        new Config(tabs);
}   

void Config::configCase(int state)
{
    if(state)
        Main::caseflag = Qt::CaseSensitive;
    else
        Main::caseflag = Qt::CaseInsensitive;
}

bool Config::destroy(QTabWidget *tabs, int tab)
{
    if(tab != last)
        return false;

    Config *config = (Config *)tabs->widget(tab);
    if(!config)
        return false;

    tabs->removeTab(tab);
    delete config;
    return true;
}

