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

#ifndef QT_DBUS_LIB

#include "ui_notice.h"

using namespace std;

volatile unsigned instances = 0;

Notice::Notice(Options& options, QString& title, QString& summary, QString& type) : QDialog(NULL)
{
    QString text;
    const char *theme_icon = "dialog-information";

    Ui::NoticeDialog ui;

    ui.setupUi((QDialog *)this);

    text = title;
    if(!summary.isEmpty()) {
        text += "\n";
        text += summary;
    }

    ui.Text->setText(text);

    info = new CoastalAbout(ui.tabAbout);
    info->setVersion(tr("Version: ") + dialog_version);
    info->setAbout(tr(dialog_about));
    info->setCopyright(tr("Copyright (c) ") + dialog_copyright);
    QIcon icon = windowIcon();
    QPixmap image = icon.pixmap(48, 48, QIcon::Normal, QIcon::On);
    QGraphicsScene scene;
    QGraphicsPixmapItem *item = scene.addPixmap(image);
    item->setVisible(true);
    info->setImage(scene);

    setAttribute(Qt::WA_DeleteOnClose);
    if((type == "error") || (type == "dialog-error"))
        theme_icon = "dialog-error";
    else if((type == "warning") || (type == "dialog-warning"))
        theme_icon = "dialog-warning";
        
    setWindowIcon(QIcon::fromTheme(theme_icon));

    if(options.show_icons)
        ui.Icon->setIcon(QIcon::fromTheme(theme_icon));
    else {
        ui.Icon->setVisible(false);
        ui.Icon->setEnabled(false);
        ui.Icon->hide();
    }

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowOpacity(options.notice_opacity);

    ++instances;

    connect(&timer, SIGNAL(timeout()), this, SLOT(close()));
    timer.start(options.notice_timeout);
    show();
}

Notice::~Notice()
{
    --instances;
}

unsigned Notice::count(void)
{
    return instances;
}

#endif












