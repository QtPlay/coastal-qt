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

Options::Options() :
QSettings()
{

	beginGroup("notices");
	show_icons = value("show_icons", true).toBool();
	notice_timeout = value("timeout", 5).toInt() * 1000;
	notice_opacity = value("opacity", 0.6).toReal();
	endGroup();
	beginGroup("multicast");
	group_network = value("network", "239.255.43.21").toString();
	group_port = value("port", 45654).toInt();
	group_sending = value("sending", 250).toInt();
	group_hops = value("hops", 1).toInt();
	endGroup();
}

void Options::save(void)
{
	beginGroup("notices");
	setValue("show_icons", show_icons);
	setValue("timeout", notice_timeout / 1000);
	setValue("opacity", notice_opacity);
	endGroup();
	beginGroup("multicast");
	setValue("network", group_network);
	setValue("port", group_port);
	setValue("sending", group_sending);
	setValue("hops", group_hops);
	endGroup();
	sync();
}














