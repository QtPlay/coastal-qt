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

static Ui::MainWindow ui;
static QToolButton *map[5];

static void uncheck(QToolButton *source)
{
    for(unsigned j = 0; j < 5; ++j) {
        if(map[j] == source)
            continue;
        map[j]->setChecked(false);
        map[j]->update();
    }
}

Main::Main(QWidget *parent) :
QMainWindow(parent)
{
    ui.setupUi((QMainWindow *)this);
    map[0] = ui.documents;
    map[1] = ui.applications;
    map[2] = ui.audioFiles;
    map[3] = ui.imageFiles;
    map[4] = ui.videoFiles;

    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(ui.documents, SIGNAL(pressed()), this, SLOT(docs()));
    connect(ui.actionDocuments, SIGNAL(triggered()), this, SLOT(docs()));
    connect(ui.actionDocuments, SIGNAL(triggered()),
        ui.documents, SLOT(toggle()));

    connect(ui.applications, SIGNAL(pressed()), this, SLOT(apps()));
    connect(ui.actionApplications, SIGNAL(triggered()), this, SLOT(apps()));
    connect(ui.actionApplications, SIGNAL(triggered()),
        ui.applications, SLOT(toggle()));

    connect(ui.audioFiles, SIGNAL(pressed()), this, SLOT(audio()));
    connect(ui.actionAudioFiles, SIGNAL(triggered()), this, SLOT(audio()));
    connect(ui.actionAudioFiles, SIGNAL(triggered()),
        ui.audioFiles, SLOT(toggle()));

    connect(ui.videoFiles, SIGNAL(pressed()), this, SLOT(video()));
    connect(ui.actionVideoFiles, SIGNAL(triggered()), this, SLOT(video()));
    connect(ui.actionVideoFiles, SIGNAL(triggered()),
        ui.videoFiles, SLOT(toggle()));

    connect(ui.imageFiles, SIGNAL(pressed()), this, SLOT(images()));
    connect(ui.actionImageFiles, SIGNAL(triggered()), this, SLOT(images()));
    connect(ui.actionImageFiles, SIGNAL(triggered()),
        ui.imageFiles, SLOT(toggle()));

    connect(ui.actionClear, SIGNAL(triggered()), this, SLOT(clear()));

    ui.searchText->setFocus();
}

Main::~Main()
{
}

void Main::clear(void)
{
    uncheck(NULL);
    all();
}

void Main::all(void)
{
}

void Main::docs(void)
{
    if(!ui.documents->isChecked()) {
        uncheck(ui.documents);
    }
    else
        all();
}

void Main::apps(void)
{
    if(!ui.applications->isChecked()) {
        uncheck(ui.applications);
    }
    else
        all();
}

void Main::audio(void)
{
    if(!ui.audioFiles->isChecked()) {
        uncheck(ui.audioFiles);
    }
    else
        all();
}

void Main::video(void)
{
    if(!ui.videoFiles->isChecked()) {
        uncheck(ui.videoFiles);
    }
    else
        all();
}

void Main::images(void)
{
    if(!ui.imageFiles->isChecked()) {
        uncheck(ui.imageFiles);
    }
    else
        all();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Main w;

    QCoreApplication::setOrganizationName("GNU Telephony");
    QCoreApplication::setOrganizationDomain("gnutelephony.org");
    QCoreApplication::setApplicationName("coastal-search");

    QTranslator translator;
    translator.load(QLocale::system().name(), TRANSLATIONS);
    app.installTranslator(&translator);

    w.show();
    return app.exec();
}

