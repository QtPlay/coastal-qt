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

Main::Main(QWidget *parent) :
QMainWindow(parent)
{
    ui.setupUi((QMainWindow *)this);
    ui.statusbar->showMessage(tr("loading..."));

    setWindowIcon(QIcon(":/manpager.png"));

    QSettings settings;
    const char *separator = ":";
#ifdef  _MSC_VER
    char buffer[256];
    buffer[0] = 0;
    GetEnvironmentVariable("MANPATH", buffer, sizeof(buffer));
    QString manpath = buffer;
    if(strchr(buffer, ';'))
        separator = ";";
#else
    QString manpath = getenv("MANPATH");
#endif

    manpath = settings.value(manpath, "manpath").toString();
#ifdef  _MSC_VER
    if(manpath.isEmpty())
        manpath = "c:\\tools\\man";
#else
    if(manpath.isEmpty())
        manpath = "/usr/share/man:/usr/local/share/man";
#endif

    manpaths = manpath.split(separator, QString::SkipEmptyParts);

    ui.actionQuit->setIcon(QIcon::fromTheme("exit"));
    ui.actionReload->setIcon(QIcon::fromTheme("reload"));
    ui.actionAbout->setIcon(QIcon::fromTheme("help-about"));

    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    ui.searchBox->setFocus();
    ui.statusbar->showMessage(tr("ready"));
}

Main::~Main()
{
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("GNU Telephony");
    QCoreApplication::setOrganizationDomain("gnutelephony.org");
    QCoreApplication::setApplicationName("coastal-manpager");

    QTranslator translator;
    translator.load(QLocale::system().name(), TRANSLATIONS);
    app.installTranslator(&translator);

    if(!QIcon::hasThemeIcon("reload"))
        QIcon::setThemeName("coastal");

    Main w;
    w.show();
    return app.exec();
}
