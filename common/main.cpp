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

#include <config.h>
#include <coastal.h>
#include <ui_about.h>

CoastalMain::CoastalMain() :
QMainWindow(NULL)
{
    program_version = VERSION;
    program_about = program_name = "Coastal Application";
    program_copyright = "2011 David Sugar";

    url_support = "https://github.com/dyfet/coastal-qt/issues";
}

void CoastalMain::support(void)
{
    Coastal::browser(url_support);
}

void CoastalMain::about(void)
{
    QString title = tr("About ") + program_name;
    CoastalAbout info(this);
    info.setWindowTitle(title);
    info.setVersion(tr("Version: ") + program_version);
    info.setAbout(tr(program_about));
    info.setCopyright(tr("Copyright (c) ") + program_copyright);
    QIcon icon = windowIcon();
    QPixmap image = icon.pixmap(48, 48, QIcon::Normal, QIcon::On);
    QGraphicsScene scene;
    QGraphicsPixmapItem *item = scene.addPixmap(image);
    item->setVisible(true);
    info.setImage(scene);
    info.exec();
}

#ifdef  WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShow)
{
    int argc, pos;
    char **argv;
    argc = 1;
    size_t len = strlen(lpCmdLine);

    for (unsigned i = 0; i < len; i++) {
        while (lpCmdLine[i] == ' ' && i < len)
            ++i;
        if (lpCmdLine[i] == '\"') {
            ++i;
            while (lpCmdLine[i] != '\"' && i < len)
                ++i;
        }
        else while (lpCmdLine[i] != ' ' && i < len)
            ++i;
        ++argc;
    }

    argv = (char **)malloc(sizeof(char *) * (argc + 1));
    argv[0] = (char*)malloc(1024);
    ::GetModuleFileName(0, argv[0], 1024);

    for(unsigned i = 1; i < (unsigned)argc; i++)
        argv[i] = (char*)malloc(len + 10);

    argv[argc] = 0;

    pos = 0;
    argc = 1;
    for (unsigned i = 0; i < len; i++) {
        while (lpCmdLine[i] == ' ' && i < len)
            ++i;
        if (lpCmdLine[i] == '\"') {
            ++i;
            while (lpCmdLine[i] != '\"' && i < len) {
                argv[argc][pos] = lpCmdLine[i];
                ++i;
                ++pos;
            }
        }
        else {
            while (lpCmdLine[i] != ' ' && i < len) {
                argv[argc][pos] = lpCmdLine[i];
                ++i;
                ++pos;
            }
        }
        argv[argc][pos] = 0;
        argc++;
        pos = 0;
    }
    return main(argc, argv);
}
#endif
