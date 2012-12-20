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

Options::Options()
{
	QSettings settings;

	modified = false;
	show_icons = settings.value("show_icons", true).toBool();
	timeout = settings.value("timeout", 5).toInt() * 1000;
	opacity = settings.value("opacity", 0.6).toReal();
}

Options::~Options()
{
	if(!modified)
		return;

	QSettings settings;
	settings.setValue("show_icons", show_icons);
	settings.setValue("timeout", timeout / 1000);
	settings.setValue("opacity", opacity);
}


















