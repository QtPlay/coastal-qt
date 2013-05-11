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
#include <time.h>

using namespace std;

class Source
{
public:
    QHostAddress host;
    unsigned seq;
};

class Message
{
public:
    Message(char *msg, size_t mlen);

    unsigned count;
    char data[512];
    size_t size;
};

Message::Message(char *msg, size_t mlen)
{
    memcpy(data, msg, mlen);
    size = mlen;
    count = 0;
}


static QList<Message *> outgoing;
static QHash<Source, time_t> incoming;
static Multicast *net;

static bool operator==(const Source& s1, const Source& s2)
{
    return s1.host.toString() == s2.host.toString() && s1.seq == s2.seq;
}

static uint qHash(const Source& key)
{
    return qHash(key.host.toString()) ^ key.seq;
}

Multicast::Multicast(Options& options, QWidget *parent) :
QUdpSocket(parent)
{
    net = this;
    addr = options.group_network;
    port = options.group_port;

    bind(options.group_port, ShareAddress);
    joinMulticastGroup(addr);
    setSocketOption(QAbstractSocket::MulticastTtlOption, options.group_hops);

    connect(this, SIGNAL(readyRead()),
	this, SLOT(process()));

    send_timer = new QTimer(this);
    connect(send_timer, SIGNAL(timeout()),
	this, SLOT(deliver()));

    send_timer->start(options.group_sending);

    expire_timer = new QTimer(this);
    connect(expire_timer, SIGNAL(timeout()),
	this, SLOT(expire()));

    connect(this, SIGNAL(user(const char *, QHostAddress)),
	parent, SLOT(user(const char *, QHostAddress))); 

    connect(this, SIGNAL(chat(const char *)),
	parent, SLOT(chat(const char *)));
}

void Multicast::process()
{
    Source from;
    quint16 recv_port;
    time_t now;
    qint64 size;

    time(&now);
    while(hasPendingDatagrams()) {
	size = readDatagram(buffer, sizeof(buffer) - 1, &from.host, &recv_port);
	if(size < 1) {
	    qDebug() << "invalid packet";
	    continue;
	}
	buffer[size] = 0;
	if(port != recv_port) {
	    qDebug() << "invalid port " << recv_port;
	    continue;
	}
	if(buffer[2] != 0) {
	    qDebug() << "invalid protocol version " << (unsigned)buffer[2];
	    continue;
	}
	from.seq = buffer[1] * 256 + buffer[0];
	if(incoming.contains(from))
	    continue;
	incoming[from] = now;
	switch(buffer[3]) {
	case USER_IDLE:
	case USER_BUSY:
	case USER_AWAY:
	    emit user(buffer, from.host);
	    break;
	case CHAT_PUBLIC:
	    emit chat(buffer);
	    break;
	default:
	    qDebug() << "unknown msg type " << (unsigned)buffer[3];
	}
    }
}

void Multicast::expire()
{
    time_t now;
    time(&now);
    QHash<Source, time_t>::iterator i = incoming.begin();
    while(i != incoming.end()) {
	QHash<Source, time_t>::iterator prior = i;
	++i;
	if(prior.value() + 9 < now) {
	    incoming.erase(prior);
	}
    }
}

void Multicast::deliver()
{
    int pos = 0;
    time_t now;

    time(&now);
    while(pos < outgoing.count()) {
	Message *msg = outgoing.at(pos);
	if(msg->count++ > 3) {
	    outgoing.removeAt(pos);
	    delete msg;
	}
	else {
	    writeDatagram(msg->data, msg->size, addr, port);
	    ++pos;
	}
    }
}		

void Multicast::send(char *msg, size_t mlen, bool immediate)
{
    static unsigned seq = 0;

    msg[0] = seq / 256;
    msg[1] = seq % 256;
    msg[2] = 0;				// protocol 0
    ++seq;
    if(immediate)
	net->writeDatagram(msg, mlen, net->addr, net->port);
    else
	outgoing.append(new Message(msg, mlen));
}	
