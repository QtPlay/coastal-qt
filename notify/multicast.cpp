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
	time_t when;
	size_t size;
};

Message::Message(char *msg, size_t mlen)
{
	memcpy(data, msg, mlen);
	size = mlen;
	count = 0;
	time(&when);
}


static QList<Message *> outgoing;
static QHash<Source, Message *> incoming;

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
	addr = options.group_network;
	port = options.group_port;

	bind(options.group_port, ShareAddress);
	joinMulticastGroup(addr);

	connect(this, SIGNAL(readyRead()),
		this, SLOT(process()));

	send_timer = new QTimer(this);
	connect(send_timer, SIGNAL(timeout()),
		this, SLOT(deliver()));

	send_timer->start(options.group_sending);
}

void Multicast::process()
{
	while(hasPendingDatagrams()) {
		size = readDatagram(buffer, sizeof(buffer));
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

void Multicast::send(char *msg, size_t mlen)
{
	static unsigned seq = 0;

	msg[0] = seq / 256;
	msg[1] = seq % 256;
	++seq;
	outgoing.append(new Message(msg, mlen));
}	
