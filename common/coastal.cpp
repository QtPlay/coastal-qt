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

#include <coastal-qt-config.h>
#include <coastal.h>
#include <ui_about.h>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef HAVE_LIBXSS
#include <QX11Info>
#include <X11/extensions/scrnsaver.h>
#endif

#ifdef  HAVE_MAGIC
#include <magic.h>
#endif

bool Coastal::env(const char *id, char *buffer, size_t size)
{
    buffer[0] = 0;
#ifdef  Q_OS_WIN
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

QString Coastal::env(const QString& id)
{
    QString result;
    char buffer[128];

    if(env(id.toAscii().constData(), buffer, sizeof(buffer)))
        result = buffer;

    return result;
}

bool Coastal::open(const QString& url)
{
    QUrl uri;

    if(url[0] == QChar('/'))
        uri = QUrl::fromLocalFile(url);
#ifdef Q_OS_WIN
    else if((url[1] == QChar(':')) || (url[0] == QChar('\\')))
        uri = QUrl::fromLocalFile(url);
#endif
    else
        uri = QUrl::fromLocalFile(QDir::currentPath() + QDir::separator() + url);

    if(QDesktopServices::openUrl(uri))
        return true;

#ifndef Q_OS_WIN
    static const char *open[] = {"xdg-open", "exo-open", "gnome-open", "kde-open", NULL};
    unsigned index = 0;
    const char *cp;
    char *argv[3];
    int status;
    QByteArray name = url.toUtf8();

    while(NULL != (cp = open[index++])) {
        pid_t pid = fork();
        if(pid) {
            waitpid(pid, &status, 0);
            if(WIFEXITED(status) && !WEXITSTATUS(status))
                return true;
            continue;
        }
        argv[0] = (char *)cp;
        argv[1] = name.data();
        argv[2] = NULL;
        execvp(*argv, argv);
        exit(-1);
    }

#endif
    // eventually use env EDITOR...

    return false;
}

bool Coastal::browser(const QString& url)
{
    QUrl web = url;

    if(!web.isValid() || web.isRelative())
        return false;

    if(QDesktopServices::openUrl(web))
        return true;

#ifndef Q_OS_WIN
    static const char *open[] = {"xdg-open", "exo-open", "gnome-open", "kde-open", NULL};
    unsigned index = 0;
    const char *cp;
    char *argv[3];
    int status;
    QByteArray uri = url.toUtf8();

    while(NULL != (cp = open[index++])) {
        pid_t pid = fork();
        if(pid) {
            waitpid(pid, &status, 0);
            if(WIFEXITED(status) && !WEXITSTATUS(status))
                return true;
            continue;
        }
        argv[0] = (char *)cp;
        argv[1] = uri.data();
        argv[2] = NULL;
        execvp(*argv, argv);
        exit(-1);
    }

#endif

    return false;
}

#if defined(Q_OS_WIN)

bool Coastal::idle(void)
{
    BOOL active = FALSE;
    SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &active, 0);

    if(active == TRUE)
        return true;

    return false;
}

#elif defined(HAVE_LIBXSS)

bool Coastal::idle(void)
{
    bool result = false;

    Display *display = QX11Info::display();
    int event, error;
    if(!XScreenSaverQueryExtension(display, &event, &error))
        return false;
    XScreenSaverInfo *info = XScreenSaverAllocInfo();
    if(info) {
        XScreenSaverQueryInfo(display, DefaultRootWindow(display), info);
        if(info->state == ScreenSaverOn)
            result = true;
        XFree(info);
    }
    return result;
}

#else

bool Coastal::idle(void)
{
    return false;
}

#endif

#if defined(QT_DBUS_LIB)

bool Coastal::notify(const QString& title, const QString& body, const QString& icon)
{
    CoastalNotify notify(title, body, icon);
    return notify.update();
}

#elif defined(Q_OS_WIN)

bool Coastal::notify(const QString& title, const QString& body, const QString& icon)
{
    QByteArray _title = title.toUtf8();
    QByteArray _body = body.toUtf8();
    QByteArray _icon = icon.toUtf8();
    bool result = true;

    char buf[512];
    snprintf(buf, sizeof(buf), "notify\t%s\t%s\t%s\n",
        _title.data(), _body.data(), _icon.data());

    HANDLE fd = CreateFile("\\\\.\\mailslot\\notify_ctrl", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(fd == INVALID_HANDLE_VALUE)
        return false;

    if(!WriteFile(fd, buf, (DWORD)strlen(buf), NULL, NULL))
        result = false;

    CloseHandle(fd);

    return result;
}

#else

bool Coastal::notify(const QString& title, const QString& body, const QString& icon)
{
    QByteArray _title = title.toUtf8();
    QByteArray _body = body.toUtf8();
    QByteArray _icon = icon.toUtf8();

    char buf[512];
    snprintf(buf, sizeof(buf), "%s\t%s\t%s\n",
        _title.data(), _body.data(), _icon.data());

    char *env = getenv("HOME");
    if(!env)
        return false;

    char path[512];
    snprintf(path, sizeof(path), "%s/.coastal-notify", env);
    int fd = ::open(path, O_WRONLY);
    if(fd < 0)
        return false;

    ::write(fd, buf, strlen(buf));
    ::close(fd);
    return true;
}

#endif

#ifdef  HAVE_MAGIC
#ifndef RUNTIME_MAGIC
#define RUNTIME_MAGIC   NULL
#endif

QString Coastal::mimefile(const QString& filename)
{
    static magic_t mdb = NULL;

    if(!mdb) {
        mdb = magic_open(MAGIC_ERROR|MAGIC_MIME_TYPE|MAGIC_SYMLINK|MAGIC_PRESERVE_ATIME);
        magic_load(mdb, RUNTIME_MAGIC);
    }
    QByteArray fn = filename.toUtf8();
    return magic_file(mdb, fn.data());
}

#else

QString Coastal::mimefile(const QString& filename)
{
    QString ext = extension(filename);

    // we should do windows registry here...

#ifdef  Q_OS_WIN
#endif

    // fallback to minetype...
    return mimetype(ext);
}

#endif

QString Coastal::extension(const QString& filename)
{
    int pos = filename.lastIndexOf(QChar('/'));
#ifdef  Q_OS_WIN
    int alt = filename.lastIndexOf(QChar('\\'));
    if(alt < 0)
        alt = filename.lastIndexOf(QChar(':'));
    if(alt > pos)
        pos = alt;
#endif

    if(pos < 0)
        pos = 0;

    int exp = filename.indexOf(QChar('.'), -1);
    if(exp < pos)
        return NULL;

    return filename.mid(exp);
}

QString Coastal::mimetype(const QString& ext)
{
    if(ext.compare(".txt", Qt::CaseInsensitive) ||
       ext.compare(".text", Qt::CaseInsensitive) ||
       ext.compare(".log", Qt::CaseInsensitive))
        return "text/plain";

    if(ext.compare(".htm", Qt::CaseInsensitive) ||
       ext.compare(".html", Qt::CaseInsensitive) ||
       ext.compare(".htx", Qt::CaseInsensitive))
        return "text/html";

    if(ext.compare(".jpg", Qt::CaseInsensitive) ||
       ext.compare(".jpeg", Qt::CaseInsensitive))
        return "image/jpeg";

    if(ext.compare(".png", Qt::CaseInsensitive))
        return "image/png";

    if(ext.compare(".bmp", Qt::CaseInsensitive))
        return "image/bmp";

    if(ext.compare(".aif", Qt::CaseInsensitive) ||
       ext.compare(".aiff", Qt::CaseInsensitive))
        return "audio/aiff";

    if(ext.compare(".au", Qt::CaseInsensitive))
        return "audio/basic";

    if(ext.compare(".gsm", Qt::CaseInsensitive))
        return "audio/x-gsm";

    if(ext.compare(".wav", Qt::CaseInsensitive))
        return "audio/wav";

    if(ext.compare(".mp3", Qt::CaseInsensitive))
        return "audio/mpeg3";

    if(ext.compare(".mid", Qt::CaseInsensitive) ||
       ext.compare(".midi", Qt::CaseInsensitive))
        return "audio/x-midi";

    if(ext.compare(".mpg", Qt::CaseInsensitive) ||
       ext.compare(".mpeg", Qt::CaseInsensitive))
        return "video/mpeg";

    if(ext.compare(".avi", Qt::CaseInsensitive))
        return "video/avi";

    if(ext.compare(".mov", Qt::CaseInsensitive))
        return "video/quicktime";

    if(ext.compare(".xml", Qt::CaseInsensitive))
        return "application/xml";

    if(ext.compare(".zip", Qt::CaseInsensitive))
        return "application/zip";


    return NULL;
}

