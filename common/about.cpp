// Copyright (C) 2011-2013 David Sugar, Tycho Softworks
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

#include <coastal.h>
#include <ui_about.h>

Ui::AboutDialog ui;

CoastalAbout::CoastalAbout(QWidget *parent) :
QDialog(parent)
{
    ui.setupUi((QDialog *)this);
    if(!parent) {
        ui.closeButton->setHidden(true);
    }
}

void CoastalAbout::setVersion(const QString& version)
{
    ui.labelVersion->setText(version);
}

void CoastalAbout::setAbout(const QString& about)
{
    ui.labelAbout->setText(about);
}

void CoastalAbout::setCopyright(const QString& copyright)
{
    ui.labelCopyright->setText(copyright);
}

void CoastalAbout::setImage(QGraphicsScene& scene)
{
    ui.graphicsView->setStyleSheet("background: transparent");
    ui.graphicsView->setScene(&scene);
}

