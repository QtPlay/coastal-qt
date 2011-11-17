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

Main::Main(const char *prefix) :
CoastalMain()
{
    ui.setupUi((QMainWindow *)this);
    ui.statusbar->showMessage(tr("loading..."));

    setWindowIcon(QIcon(":/search.png"));

    map[0] = ui.documents;
    map[1] = ui.applications;
    map[2] = ui.audioFiles;
    map[3] = ui.imageFiles;
    map[4] = ui.videoFiles;

    if(prefix)
        QDir::setCurrent(prefix);

    dir.setPath(QDir::currentPath());
    history.append(dir.path());

    QSettings settings;

    int paths = settings.beginReadArray("paths");
//  qDebug() << "SIZE " << paths << endl;
    for(int path = 0; path < paths; ++path) {
        settings.setArrayIndex(path);
        QString temp = settings.value("path").toString();
//      qDebug() << "STR " << path << " VALUE " << temp << endl;
        if(temp == dir.path())
            continue;
        history.append(temp);
    }
    settings.endArray();

    ui.actionQuit->setIcon(QIcon::fromTheme("exit"));
    ui.actionReload->setIcon(QIcon::fromTheme("reload"));
    ui.actionClear->setIcon(QIcon::fromTheme("editclear"));
    ui.actionAbout->setIcon(QIcon::fromTheme("help-about"));

    ui.applications->setIcon(QIcon::fromTheme("application-x-executable"));
    ui.documents->setIcon(QIcon::fromTheme("text-x-generic"));
    ui.audioFiles->setIcon(QIcon::fromTheme("audio-x-generic"));
    ui.imageFiles->setIcon(QIcon::fromTheme("image-x-generic"));
    ui.videoFiles->setIcon(QIcon::fromTheme("video-x-generic"));

    ui.actionApplications->setIcon(QIcon::fromTheme("application-x-executable"));
    ui.actionDocuments->setIcon(QIcon::fromTheme("text-x-generic"));
    ui.actionAudioFiles->setIcon(QIcon::fromTheme("audio-x-generic"));
    ui.actionImageFiles->setIcon(QIcon::fromTheme("image-x-generic"));
    ui.actionVideoFiles->setIcon(QIcon::fromTheme("video-x-generic"));

    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));

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
    connect(ui.pathButton, SIGNAL(clicked()), this, SLOT(changeDir()));
    connect(ui.pathBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectDir(int)));

    // adding history triggers selectDir...
    ui.pathBox->addItems(history);
}

Main::~Main()
{
    QSettings settings;
    int pos = 0;

    settings.beginWriteArray("paths");
    while(pos < history.size()) {
        settings.setArrayIndex(pos);
        settings.setValue("path", history[pos++]);
    }
    settings.endArray();
}

void Main::selectDir(int index)
{
    if(index < 0)
        return;

    QString path = history.takeAt(index);
    QDir::setCurrent(path);
    dir.setPath(QDir::currentPath());
//  qDebug() << "SELECT PATH " << path << " DIR " << dir.path() << endl;
    history.insert(0, dir.path());

    uncheck(NULL);

    ui.listFiles->clear();
    ui.searchText->setFocus();
    ui.statusbar->showMessage(tr("ready"));
}

void Main::changeDir(void)
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Directory"), dir.path());

    if(path.isNull() == false) {
        QDir::setCurrent(path);
        dir.setPath(QDir::currentPath());

//      qDebug() << "CHANGE PATH " << path << " DIR " << dir.path() << endl;
        unsigned pos = history.size();
        while(pos > 0) {
            --pos;
            if(dir.path() == history[pos])
                history.takeAt(pos);
        }

        // no more than 10 elements...
        while(history.size() > 9)
            history.takeLast();

        history.insert(0, dir.path());

        // triggers select dir...
        ui.pathBox->clear();
        ui.pathBox->addItems(history);

        // rest in selectDir
    }
}

void Main::clear(void)
{
    uncheck(NULL);
    all();
}

void Main::all(void)
{
    if(!ui.listFiles->count()) {
        ui.searchText->setFocus();
        ui.statusbar->showMessage(tr("ready"));
        return;
    }

    ui.statusbar->showMessage(tr("selected"));
}

void Main::docs(void)
{
    if(!ui.documents->isChecked()) {
        uncheck(ui.documents);
        ui.statusbar->showMessage(tr("documents"));
    }
    else
        all();
}

void Main::apps(void)
{
    if(!ui.applications->isChecked()) {
        uncheck(ui.applications);
        ui.statusbar->showMessage(tr("applications"));
    }
    else
        all();
}

void Main::audio(void)
{
    if(!ui.audioFiles->isChecked()) {
        uncheck(ui.audioFiles);
        ui.statusbar->showMessage(tr("audio files"));
    }
    else
        all();
}

void Main::video(void)
{
    if(!ui.videoFiles->isChecked()) {
        uncheck(ui.videoFiles);
        ui.statusbar->showMessage(tr("video files"));
    }
    else
        all();
}

void Main::images(void)
{
    if(!ui.imageFiles->isChecked()) {
        uncheck(ui.imageFiles);
        ui.statusbar->showMessage(tr("image files"));
    }
    else
        all();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("GNU Telephony");
    QCoreApplication::setOrganizationDomain("gnutelephony.org");
    QCoreApplication::setApplicationName("coastal-search");

    QTranslator translator;
    translator.load(QLocale::system().name(), TRANSLATIONS);
    app.installTranslator(&translator);

    if(!QIcon::hasThemeIcon("reload"))
        QIcon::setThemeName("coastal");

    Main w(argv[1]);
    w.show();
    return app.exec();
}

