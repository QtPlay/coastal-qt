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
#include "ui_notice.h"

using namespace std;

Notice::Notice(const char *title, const char *summary, const char *type) : QDialog(NULL)
{
    QString text;

    Ui::NoticeDialog *ui = new Ui::NoticeDialog;

    ui->setupUi((QDialog *)this);

    text = title;
    if(summary) {
        text += "\n";
        text += summary;
    }

    ui->Text->setText(text);

    setAttribute(Qt::WA_DeleteOnClose);
    QIcon icon;
    if(type && !strcmp(type, "error")) 
        icon = QIcon::fromTheme("dialog-error");
    else if(type && !strcmp(type, "warning"))
        icon = QIcon::fromTheme("dialog-warning");
    else
        icon = QIcon::fromTheme("dialog-information");

    QPixmap image = icon.pixmap(16, 16, QIcon::Normal, QIcon::On);
    QGraphicsScene scene;
    QGraphicsPixmapItem *item = scene.addPixmap(image);
    item->setVisible(true);

    ui->Icon->setStyleSheet("background: transparent");
    ui->Icon->setScene(&scene);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowOpacity(qreal(0.60));

    connect(&timer, SIGNAL(timeout()), this, SLOT(close()));
    timer.start(5000);
    show();
}














