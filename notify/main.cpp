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
#include "ui_main.h"

using namespace std;

static Ui::MainWindow ui;

Main::Main() :
CoastalMain()
{
    ui.setupUi((QMainWindow *)this);

    program_name = "Coastal Notify";
    program_about = "Coastal Notifications";
    setWindowIcon(QIcon(":/notify.png"));
    setWindowTitle(program_name);
    setWindowFlags(Qt::Window);

    QApplication::setQuitOnLastWindowClosed(true);

    connect(ui.aboutButton, SIGNAL(clicked()), this, SLOT(about()));

    connect(ui.quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    trayicon = new QSystemTrayIcon(this);
    if(!trayicon) {
        show();
        return;
    }

    QMenu *traymenu = new QMenu(NULL);

    QAction *aboutAction = new QAction(tr("&About"), this);
    aboutAction->setIcon(QIcon::fromTheme("help-about"));
    aboutAction->setIconVisibleInMenu(true);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    traymenu->addAction(aboutAction);

    QAction *quitAction = new QAction(tr("&Quit"), this);
    quitAction->setIcon(QIcon::fromTheme("exit"));
    quitAction->setIconVisibleInMenu(true);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    traymenu->addAction(quitAction);

    trayicon->setContextMenu(traymenu);
    trayicon->setIcon(QIcon(":/notify.png"));
    trayicon->show();
    
    connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(action(QSystemTrayIcon::ActivationReason)));
}

Main::~Main()
{
}

void Main::action(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if(isVisible())
            hide();
        else if(isEnabled())
            show();
        return;
    default:
        return;
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    bool result;
    bool internal = false;

    if(argv[1] && !strcmp(argv[1], "-i")) {
        internal = true;
        ++argv;
    }

    if(argv[1] && !argv[2]) {
        fprintf(stderr, "use: coastal-notify [[-i] \"title\" \"summary\" [icon]]\n");
        return 2;
    }

    if(argv[1] && !internal) {
        if(argv[2] && argv[3])
            result = Coastal::notify(argv[1], argv[2], argv[3]);
        else if(argv[2])
            result = Coastal::notify(argv[1], argv[2]);

        if(result)
            return 0;
    }

    QCoreApplication::setOrganizationName("GNU Telephony");
    QCoreApplication::setOrganizationDomain("gnutelephony.org");
    QCoreApplication::setApplicationName("coastal-notify");

    QTranslator translator;
    translator.load(QLocale::system().name(), TRANSLATIONS);
    app.installTranslator(&translator);

    if(!QIcon::hasThemeIcon("reload"))
        QIcon::setThemeName("coastal");

    if(internal) {
        Options options;
        new Notice(options, argv[1], argv[2], argv[3]);
        return app.exec();
    }
    Main w;
    app.exec();
}

