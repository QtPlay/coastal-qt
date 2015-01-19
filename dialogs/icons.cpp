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

#include "icons.h"
#include "ui_icons.h"

using namespace std;

static Ui::Dialog ui;

Icons::Icons() :
CoastalDialog()
{
    ui.setupUi((QDialog *)this);

    QApplication::setQuitOnLastWindowClosed(true);

    connect(ui.close, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(ui.name, SIGNAL(textChanged(const QString&)),
        this, SLOT(input(const QString&)));

	ui.image->setToolButtonStyle(Qt::ToolButtonIconOnly);
	ui.image->setIconSize(QSize(64, 64));

    show();
}

void Icons::input(const QString& text)
{
    if(QIcon::hasThemeIcon(text)) {
		QPixmap pix = QIcon::fromTheme(text).pixmap(64);
        ui.image->setIcon(pix);
	}
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if(argv[1] && argv[2]) {
        fprintf(stderr, "use: coastal-icons [theme]\n");
        return 2;
    }

    QCoreApplication::setOrganizationName("GNU Telephony");
    QCoreApplication::setOrganizationDomain("gnutelephony.org");
    QCoreApplication::setApplicationName("coastal-icons");

    if(argv[1])
        QIcon::setThemeName(argv[1]);

    Icons w;
    app.exec();
}

