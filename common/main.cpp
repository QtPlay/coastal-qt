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

#include <config.h>
#include <coastal.h>
#include <ui_about.h>

CoastalMain::CoastalMain() :
QMainWindow(NULL)
{
    program_version = VERSION;
    program_about = program_name = "Coastal Application";
    program_copyright = "2011 David Sugar";
}

void CoastalMain::about(void)
{
    QString title = tr("About ") + program_name;
    CoastalAbout info(this);
    info.setWindowTitle(title);
    info.setVersion(tr("Version: ") + program_version);
    info.setAbout(tr(program_about));
    info.setCopyright(tr("Copyright (c) ") + program_copyright);
    QIcon icon = windowIcon();
    QPixmap image = icon.pixmap(48, 48, QIcon::Normal, QIcon::On);
    QGraphicsScene scene;
    QGraphicsPixmapItem *item = scene.addPixmap(image);
    item->setVisible(true);
    info.setImage(scene);
    info.exec();
}
