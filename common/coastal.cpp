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

#include "../config.h"
#include <coastal.h>
#include <ui_about.h>

#ifndef WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif

bool Coastal::env(const char *id, char *buffer, size_t size)
{
    buffer[0] = 0;
#ifdef  WIN32
    if(!GetEnvironmentVariable(id, buffer, size))
        return false;
    return true;
#else
    const char *cp = getenv(id);
    if(!cp)
        return false;
    snprintf(buffer, size, "%s", cp);
    return true;
#endif
}

bool Coastal::open(const char *url)
{
    QUrl uri;

    if(*url == '/')
        uri = QUrl::fromLocalFile(QString(url));
#ifdef WIN32
    else if((url[1] == ':') || (*url == '\\'))
        uri = QUrl::fromLocalFile(QString(url));
#endif
    else
        uri = QUrl::fromLocalFile(QDir::currentPath() + QDir::separator() + url);

    if(QDesktopServices::openUrl(uri))
        return true;

#ifndef WIN32
    static const char *open[] = {"xdg-open", "exo-open", "gnome-open", "kde-open", NULL};
    unsigned index = 0;
    const char *cp;
    char *argv[3];
    int status;

    while(NULL != (cp = open[index++])) {
        pid_t pid = fork();
        if(pid) {
            waitpid(pid, &status, 0);
            if(WIFEXITED(status) && !WEXITSTATUS(status))
                return true;
            continue;
        }
        argv[0] = (char *)cp;
        argv[1] = (char *)url;
        argv[2] = NULL;
        execvp(*argv, argv);
        exit(-1);
    }

#endif
    // eventually use env EDITOR...

    return false;
}

bool Coastal::browser(const char *url)
{
    QUrl web = QString(url);

    if(!web.isValid() || web.isRelative())
        return false;

    if(QDesktopServices::openUrl(web))
        return true;

#ifndef WIN32
    static const char *open[] = {"xdg-open", "exo-open", "gnome-open", "kde-open", NULL};
    unsigned index = 0;
    const char *cp;
    char *argv[3];
    int status;

    while(NULL != (cp = open[index++])) {
        pid_t pid = fork();
        if(pid) {
            waitpid(pid, &status, 0);
            if(WIFEXITED(status) && !WEXITSTATUS(status))
                return true;
            continue;
        }
        argv[0] = (char *)cp;
        argv[1] = (char *)url;
        argv[2] = NULL;
        execvp(*argv, argv);
        exit(-1);
    }

#endif

    return false;
}

