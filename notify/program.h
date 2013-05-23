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

#ifndef MAIN_H_
#define MAIN_H_

#include <coastal-qt-config.h>
#include <coastal.h>
#include <QtNetwork>

typedef enum
{
    MSG_NOTICE = 0,
    MSG_WARNING,
    MSG_ERROR,
    MSG_FATAL,

	USER_BUSY = 10,
    USER_IDLE,
    USER_AWAY,
	USER_EXPIRES,

	CHAT_PUBLIC = 20
} msgtype_t;

enum {
	ITEM_ID = Qt::DisplayRole,
	ITEM_TEXT = Qt::UserRole + 1,
	ITEM_SERIES,
	ITEM_USER,
	ITEM_SIZE	
};

class Options : private QSettings
{
Q_OBJECT

public:
    unsigned group_sending;
    unsigned group_port;
	unsigned group_hops;
    QString group_network;
    qreal notice_opacity;
    unsigned notice_timeout;
    bool show_icons;
	unsigned max_lines;

    Options();

    void save(void);
};

class Fifo : public QThread
{
Q_OBJECT

private:
    void process(char *buf);

public:
    Fifo();

    void stop(void);

    void run(void);

signals:
    void notice(QString title, QString summary, QString icon);
    void restart(void);
};

class Multicast : public QUdpSocket
{
    Q_OBJECT

private:
    QTimer *send_timer, *expire_timer;
    QHostAddress addr;
    quint16 port;
    char buffer[1024];

public:
    Multicast(Options& options, QWidget *controller);

    static void send(char *msg, size_t mlen, bool immediate = false);

private slots:
    void process();
    void deliver();
    void expire();

signals:
	void user(const char *msg, QHostAddress from);
	void chat(const char *msg);
};

class UserItem : public QListWidgetItem
{
public:
	UserItem(const QString& id);

	QHostAddress from;
	time_t	active;
	time_t	update;
};

class ChatItem : public QListWidgetItem
{
public:
	ChatItem(const QString& id, const QString& msg);

	QSize size;
	unsigned series;
};

class ChatDisplay : public QStyledItemDelegate
{
public:
    ChatDisplay(QObject *parent = NULL);

    void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

class Main : public CoastalDialog
{
Q_OBJECT

private:
	static unsigned series;
    const char *userid;
    Fifo *fifo;
    Multicast *net;
    QTimer *user_timer;
	CoastalAbout *info;

	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event);
 
public:
    static bool restart_flag;

    Options options;

    Main();
    virtual ~Main();
    
    void stop(void);

	inline static unsigned getSeries(void)
		{return series;}

public slots:
    void action(QSystemTrayIcon::ActivationReason reason);

    void notice(QString title, QString summary, QString icon = "");

    void restart(void);

	void about(void);

	void config(void);

	void menu(const QPoint& pos);

	void user(const char *msg, QHostAddress from);

	void chat(const char *msg);

private slots:
    void status(void);
	void input(void);
};

#ifndef QT_DBUS_LIB

class Notice : public QDialog
{
Q_OBJECT

private:
    QTimer timer;

public:
    Notice(Options& options, QString& title, QString& summary, QString& icon);
    ~Notice();

    static unsigned count(void);
};

#endif

#endif

