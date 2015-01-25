// Copyright (C) 2011-2015 David Sugar, Tycho Softworks
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

#include <coastal-qt-config.h>
#include <coastal.h>

CoastalConditional::CoastalConditional() : QMutex()
{
}

bool CoastalConditional::initialize(bool result)
{
	return false;
}

bool CoastalConditional::finalize(bool result)
{
	return true;
}

bool CoastalConditional::wait(unsigned long time)
{
	bool result = false, exiting = false;
	while(!exiting) {
		lock();
		exiting = initialize(result);
		if(!exiting) {
			result = cond.wait(this, time);
			exiting = finalize(result);	
		}
		unlock();
	}
	return result;
}

