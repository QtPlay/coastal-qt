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

	show_icons = value("show_icons", true).toBool();
	timeout = value("timeout", 5).toInt() * 1000;
	opacity = value("opacity", 0.6).toReal();
	network = value("network", "*").toString();
}

void Options::change(QString net)
{
	network = net;
	setValue("network", net);
	sync();
}

void Options::save(void)
{
	setValue("show_icons", show_icons);
	setValue("timeout", timeout / 1000);
	setValue("opacity", opacity);
	sync();
}


















