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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    bool result;

    if(!argv[1] || !argv[2]) {
        cerr << "use: coastal-notify \"title\" \"summary\" [icon]" << endl;
        return 2;
    }

    if(argv[3])
        result = Coastal::notify(argv[1], argv[2], argv[3]);
    else
        result = Coastal::notify(argv[1], argv[2]);

    if(result)
        return 0;

    return 1;
}

