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

#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <time.h>

using namespace std;

static Ui::MainDialog ui;

bool Main::restart_flag = false;
int Main::series = 1;

Main::Main() :
CoastalDialog()
{
    ui.setupUi((QDialog *)this);

    dialog_name = "Coastal Notify";
    dialog_about = "Coastal Notifications and Multicast Chat";

    setWindowIcon(QIcon(":/notify.png"));
    setWindowTitle(dialog_name);
    setWindowFlags(Qt::Window);

    info = new CoastalAbout();
    info->setVersion(tr("Version: ") + dialog_version);
    info->setAbout(tr(dialog_about));
    info->setCopyright(tr("Copyright (c) ") + dialog_copyright);

    static QIcon icon = windowIcon();
    static QPixmap image = icon.pixmap(48, 48, QIcon::Normal, QIcon::On);
    static QGraphicsScene scene;
    
    QGraphicsPixmapItem *item = scene.addPixmap(image);
    item->setVisible(true);
    info->setImage(scene);
    ui.tab->addTab(info, tr("About"));
//    ui.tab->setDocumentMode(true);

    QApplication::setQuitOnLastWindowClosed(true);

    trayicon = new QSystemTrayIcon(this);
    if(trayicon) {
        QMenu *traymenu = new QMenu(NULL);
        connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
        connect(ui.actionOptions, SIGNAL(triggered()), this, SLOT(config()));
        connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
        traymenu->addAction(ui.actionAbout);
        traymenu->addAction(ui.actionOptions);
        traymenu->addAction(ui.actionQuit);
        trayicon->setContextMenu(traymenu);
        trayicon->setIcon(QIcon(":/notify.png"));
        trayicon->show();
        
        connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(action(QSystemTrayIcon::ActivationReason)));
    }
    else
        show();

    connect(ui.chatView->model(), SIGNAL(rowsInserted(const QModelIndex &,int,int)), ui.chatView, SLOT(scrollToBottom()));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(menu(const QPoint&)));

    userid = strdup(Coastal::userid().toUtf8());

    net = new Multicast(options, this);
    fifo = new Fifo();
    connect(fifo, SIGNAL(notice(QString,QString,QString)), this, SLOT(notice(QString,QString,QString)), Qt::QueuedConnection);
    connect(fifo, SIGNAL(restart()), this, SLOT(restart()), Qt::QueuedConnection);
    fifo->start();

    connect(ui.chatEdit, SIGNAL(returnPressed()), this, SLOT(input()));

    user_timer = new QTimer(this);
    connect(user_timer, SIGNAL(timeout()),
		this, SLOT(status()));

    status();   // initial posting
	user_timer->start(25000);   // and every 25 seconds after...

    ui.chatView->setItemDelegate(new ChatDisplay(ui.chatView));
}

Main::~Main()
{
    char buf[64];

    buf[3] = USER_EXPIRES;
    memcpy(buf + 4, userid, 60);
    buf[63] = 0;
    Multicast::send(buf, strlen(buf + 4) + 5, true);

    stop();
}

void Main::menu(const QPoint &pos)
{
    QMenu m;
    m.addAction(ui.actionAbout);
    m.addAction(ui.actionQuit);
    m.exec(this->mapToGlobal(pos));
}

void Main::stop(void)
{
    if(fifo) {
        fifo->stop();
        fifo = NULL;
    }
}

void Main::about(void)
{
    ui.tab->setCurrentIndex(2);
    show();
} 

void Main::config(void)
{
    ui.tab->setCurrentIndex(1);
    show();
}

void Main::restart(void)
{
    stop();
    restart_flag = true;
    qApp->quit();
}

void Main::input(void)
{
    char buf[64 + 256];
    QString line = ui.chatEdit->text();
    ui.chatEdit->clear();

    buf[2] = 0; // protocol version
    buf[3] = CHAT_PUBLIC; 
    memcpy(buf + 4, userid, 60);
    buf[63] = 0;
    snprintf(buf + 64, 256, "%s", line.toUtf8().constData());
    Multicast::send(buf, strlen(buf + 64) + 65, false);
}

void Main::chat(const char *msg)
{
    QListWidgetItem *item = new QListWidgetItem();

    while(ui.chatView->count() >= (int)options.max_lines) {
        delete ui.chatView->takeItem(0);
    }

    item->setData(ITEM_ID, msg + 4);
    item->setData(ITEM_TEXT, msg + 64);
    ui.chatView->addItem(item);
    ui.chatView->showMaximized();
}

void Main::user(const char *msg, QHostAddress from)
{
    msgtype_t mtype = (msgtype_t)msg[3];
    QString id = msg + 4; 
    UserItem *item;
    unsigned ind = ui.users->count();
    time_t now;

    time(&now);
    while(ind) {
        item = (UserItem *)(ui.users->item(--ind));
        if(item->text() == id) {
            ++ind;
            break;
        }
    } 
    if(!ind) {
        item = new UserItem(id);
        ui.users->addItem(item);
    }
    if(!item->update || mtype == USER_IDLE)
        item->from = from;
    time(&item->update);
    switch(mtype) {
    case USER_IDLE:
        time(&item->active);
        item->setIcon(QIcon(":/user-idle.png"));
        break;
    case USER_EXPIRES:
        item->setIcon(QIcon(":/user-expires.png"));
        item->update -= 90;
        break;
    case USER_AWAY:
        if(item->active + 30 < now)
            item->setIcon(QIcon(":/user-away.png"));
        break;
    case USER_BUSY:
        if(item->active + 30 < now)
            item->setIcon(QIcon(":/user-busy.png"));
        break;
    default:
        qDebug() << "invalid user message type " << mtype;
    }
}

void Main::notice(QString title, QString body, QString icon)
{
#ifdef  QT_DBUS_LIB
    Coastal::notify(title, body, icon);
#else
    new Notice(options, title, body, icon);
#endif
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

    if(argv[1] && !argv[2]) {
        fprintf(stderr, "use: coastal-notify [\"title\" \"summary\" [icon]]\n");
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

    Coastal::icons();

    Main w;
    app.exec();

#ifdef  HAVE_UNISTD_H
    if(Main::restart_flag) {
        execvp(argv[0], argv);
    }
#endif
}

void Main::status(void)
{
    char buf[64];

    if(Coastal::away())
        buf[3] = USER_AWAY;
    else
        buf[3] = USER_IDLE;

    buf[2] = 0; // protocol version
    memcpy(buf + 4, userid, 60);
    buf[63] = 0;
    Multicast::send(buf, strlen(buf + 4) + 5, true);
}      
