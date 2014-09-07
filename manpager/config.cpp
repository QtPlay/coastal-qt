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
#include "config.h"
#include "ui_config.h"
#include "main.h"

static int current = -1;
static int last = -1;
static Ui::Config ui;
static Main *main;
static QCheckBox *sections[10];

Config::Config(QTabWidget *tabs) :
QDialog()
{
    ui.setupUi((QDialog *)this);

    sections[0] = ui.section1;
    sections[1] = ui.section2;
    sections[2] = ui.section3;
    sections[3] = ui.section4;
    sections[4] = ui.section5;
    sections[5] = ui.section6;
    sections[6] = ui.section7;
    sections[7] = ui.section8;
    sections[8] = ui.sectionl;
    sections[9] = ui.sectionn;

    for(unsigned index = 0; index < 10; ++index)
        sections[index]->setChecked(Main::sections[index]->isChecked());

    int views = tabs->count();

    tabs->addTab(this, "Options");
    tabs->setCurrentIndex(views);
    tabs->setTabsClosable(true);
    last = views;

    ui.list->addItems(main->manpaths);
    ui.list->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui.list, SIGNAL(currentRowChanged(int)), this, SLOT(selected(int)));

    connect(ui.upButton, SIGNAL(clicked()), this, SLOT(up()));
    connect(ui.downButton, SIGNAL(clicked()), this, SLOT(down()));

    connect(ui.appendButton, SIGNAL(clicked()), this, SLOT(append()));
    connect(ui.removeButton, SIGNAL(clicked()), this, SLOT(remove()));

    connect(ui.acceptButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    connect(this, SIGNAL(closeRequested(int)), main, SLOT(close(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(reload()), main, SLOT(reload()), Qt::QueuedConnection);

    selected(-1);
}

Config::~Config()
{
    last = -1;
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

void Config::cancel(void)
{
    emit closeRequested(last);
}

void Config::accept(void)
{
    int count = ui.list->count();
    if(count) {
        main->manpaths.clear();
        for(int pos = 0; pos < count; ++pos) {
            QListWidgetItem *item = ui.list->item(pos);
            main->manpaths << item->text();
        }
    }
    for(unsigned index = 0; index < 10; ++index)
        Main::sections[index]->setChecked(sections[index]->isChecked());

    emit closeRequested(last);
    emit reload();
}

void Config::up(void)
{
    QListWidgetItem *item = ui.list->takeItem(current);
    ui.list->insertItem(--current, item);
    ++current;
}

void Config::down(void)
{
    QListWidgetItem *item = ui.list->takeItem(current);
    ui.list->insertItem(++current, item);
    --current;
}

void Config::remove(void)
{
    ui.list->takeItem(current);
    if(current >= ui.list->count())
        selected(-1);
}

void Config::append(void)
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Directory"), "/");

    if(path.isNull())
        return;

    ui.list->addItem(path);

    if(current < 0)
        selected(0);
}

void Config::selected(int row)
{
    current = row;

    if(row < 0 || row >= ui.list->count()) {
        ui.upButton->setEnabled(false);
        ui.downButton->setEnabled(false);
        ui.removeButton->setEnabled(false);
        return;
    }

    if(ui.list->count() > 0)
        ui.removeButton->setEnabled(true);
    else {
        ui.upButton->setEnabled(false);
        ui.downButton->setEnabled(false);
        ui.removeButton->setEnabled(false);
        return;
    }

    if(row > 0)
        ui.upButton->setEnabled(true);
    else
        ui.upButton->setEnabled(false);

    if(row < ui.list->count() - 1)
        ui.downButton->setEnabled(true);
    else
        ui.downButton->setEnabled(false);
}

void Config::create(QTabWidget *tabs, Main *top)
{
    main = top;
    if(last > -1)
        tabs->setCurrentIndex(last);
    else
        new Config(tabs);
}   

