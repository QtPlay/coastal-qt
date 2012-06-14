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

Main::Main(const char *prefix) :
CoastalMain()
{
    ui.setupUi((QMainWindow *)this);
    ui.statusbar->showMessage(tr("loading..."));

    program_name = "Coastal Search";
    program_about = "Coastal File Search Utility";
    setWindowIcon(QIcon(":/search.png"));
    setWindowTitle(program_name);

    if(prefix)
        QDir::setCurrent(prefix);

    dir.setPath(QDir::currentPath());
    history.append(dir.path());

    QSettings settings;
    resize(settings.value("size", QSize(760, 540)).toSize());
    QString filter = settings.value("filter", ".txt;.log").toString();
    ui.filterTypes->setText(filter);

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
    ui.actionSearch->setIcon(QIcon::fromTheme("search"));

    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui.actionClear, SIGNAL(triggered()), this, SLOT(clear()));
    connect(ui.actionSearch, SIGNAL(triggered()), this, SLOT(reload()));
    connect(ui.pathButton, SIGNAL(clicked()), this, SLOT(changeDir()));
    connect(ui.pathBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectDir(int)));

    connect(ui.searchText, SIGNAL(returnPressed()), this, SLOT(reload()));
    connect(ui.searchName, SIGNAL(returnPressed()), this, SLOT(reload()));
    connect(ui.filterTypes, SIGNAL(returnPressed()), this, SLOT(reload()));

    // adding history triggers selectDir...
    ui.pathBox->addItems(history);
}

Main::~Main()
{
    QSettings settings;
    int pos = 0;

    settings.setValue("size", size());
    settings.setValue("filter", ui.filterTypes->text());

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

    clear();
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
    ui.indexView->setModel(NULL);
    ui.searchText->setText("");
    ui.searchName->setText("*");
    ui.searchText->setFocus();
    ui.statusbar->showMessage(tr("ready"));
}

void Main::reload(void)
{
    ui.statusbar->showMessage(tr("reloading..."));
    ui.indexView->setModel(NULL);
    ui.statusbar->showMessage(tr("ready"));
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

