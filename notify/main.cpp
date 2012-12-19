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

using namespace std;

Main::Main(QWidget *parent) :
CoastalMain()
{
//    ui.setupUi((QMainWindow *)this);

    program_name = "Coastal Notify";
    program_about = "Coastal Notifications";
    setWindowIcon(QIcon(":/notify.png"));
    setWindowTitle(program_name);
    setWindowFlags(Qt::Window);

    trayicon = new QSystemTrayIcon(this);
    if(!trayicon) {
        show();
        return;
    }

    QMenu *traymenu = new QMenu(NULL);

    QAction *aboutAction = new QAction(tr("About"), this);
    aboutAction->setIcon(QIcon::fromTheme("help-about"));
    aboutAction->setIconVisibleInMenu(true);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    traymenu->addAction(aboutAction);

    trayicon->setContextMenu(traymenu);
    trayicon->setIcon(QIcon(":/notify.png"));
    trayicon->show();
    QApplication::setQuitOnLastWindowClosed(true);

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
            close();
//        else if(isEnabled())
//            open();
        return;
    default:
        return;
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    bool result;

    if(argv[1] && !argv[2]) {
        cerr << "use: coastal-notify [\"title\" \"summary\" [icon]]" << endl;
        return 2;
    }

    if(argv[1]) {
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

    Main w;
    return app.exec();
}

