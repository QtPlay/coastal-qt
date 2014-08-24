// Copyright (C) 2011-2013 David Sugar, Tycho Softworks
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
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#include <lmcons.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef HAVE_LIBXSS
#if QT_VERSION >= 0x050000
#include <qpa/qplatformnativeinterface.h>
#else
#include <QX11Info>
#endif
#include <X11/extensions/scrnsaver.h>
#endif

#if QT_VERSION >= 0x050000
#include <QMimeDatabase>
#include <QMimeType>
#endif

#ifdef  HAVE_MAGIC
#include <magic.h>
#endif

QString Coastal::userid()
{
#ifdef  Q_OS_WIN
    char winUserName[UNLEN + 1];
    DWORD winUserNameSize = sizeof(winUserName);

#if defined(UNICODE)
    if (qWinVersion() & Qt::WV_NT_based ) {
        GetUserName( winUserName, &winUserNameSize );
        return qt_winQString( winUserName );
    }
#endif
    GetUserNameA( winUserName, &winUserNameSize );
    return QString::fromLocal8Bit( winUserName );
#else
    return QString(getlogin());
#endif
}

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

    if(env(id.toUtf8().constData(), buffer, sizeof(buffer)))
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

bool Coastal::away(void)
{
    BOOL active = FALSE;
    SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &active, 0);

    if(active == TRUE)
        return true;

    return false;
}

#elif defined(HAVE_LIBXSS)

bool Coastal::away(void)
{
    bool result = false;

#if QT_VERSION >= 0x050000
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    if(!native)
        return false;
    Display *display = static_cast<Display *>(native->nativeResourceForWindow("display", NULL));
#else
    Display *display = QX11Info::display();
#endif
    if(!display)
        return false;

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

bool Coastal::away(void)
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

#else

bool Coastal::notify(const QString& title, const QString& body, const QString& icon)
{
    return false;
}

#endif

#ifdef  HAVE_MAGIC
#ifndef RUNTIME_MAGIC
#define RUNTIME_MAGIC   NULL
#endif

QString Coastal::mimeFile(const QString& filename)
{
    static magic_t mdb = NULL;

    if(!mdb) {
        mdb = magic_open(MAGIC_ERROR|MAGIC_MIME_TYPE|MAGIC_SYMLINK|MAGIC_PRESERVE_ATIME);
        magic_load(mdb, RUNTIME_MAGIC);
    }
    QByteArray fn = filename.toUtf8();
    return magic_file(mdb, fn.data());
}

#elif QT_VERSION >= 0x050000

QString Coastal::mimeFile(const QString& filename)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filename);

    if(mime.inherits("video/mp4"))
        return "video/mp4";
    else if(mime.inherits("video/mpeg"))
        return "video/mpeg";
    else if(mime.inherits("video/ogg"))
        return "video/ogg";
    else if(mime.inherits("video/quicktime"))
        return "video/quicktime";
    else if(mime.inherits("video/x-msvideo"))
        return "video/x-msvideo";
    else if(mime.inherits("video/x-flv"))
        return "video/x-flv";
    else if(mime.inherits("video/webm"))
        return "video/webm";
    else if(mime.inherits("image/jpeg"))
        return "image/jpeg";
    else if(mime.inherits("image/png"))
        return "image/png";
    else if(mime.inherits("audio/wav"))
        return "audio/wav";
    else if(mime.inherits("audio/basic"))
        return "audio/basic";
    else if(mime.inherits("text/html"))
        return "text/html";
    else if(mime.inherits("text/plain"))
        return "text/plain";

    QString ext = extension(filename);
    return mimeType(ext);
}

#else

QString Coastal::mimeFile(const QString& filename)
{
    QString ext = extension(filename);

    // we should do windows registry here...

#ifdef  Q_OS_WIN
#endif

    // fallback to minetype...
    return mimeType(ext);
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

QString Coastal::mimeType(const QString& ext)
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

bool Coastal::emailTo(QString& to, QString& topic, QString& body)
{
#ifndef Q_OS_WIN
    if(QFile::exists("/usr/bin/xdg-email")) {
        QString cmd = "xdg-email";
        QStringList args;
        args << to << "--subject" << topic << "--body" << body;
        return QProcess::startDetached(cmd, args);
    }
#endif

#if defined(Q_OS_MAC)
    QString url("mailto:%1?subject=%2&body=%3");
    QString etopic(QUrl::toPercentEncoding(topic));
    QString ebody(QUrl::toPercentEncoding(body));
    QString formattedContent(QString(url).arg(to).arg(etopic).arg(ebody));
    return QDesktopServices::openUrl(QUrl(url)); 
#else
    QString url("mailto:%1?subject=%2&body=%3");
    QString encoded(QUrl::toPercentEncoding(body));
    QString formattedContent(QString(url).arg(to).arg(topic).arg(encoded));
    return QDesktopServices::openUrl(QUrl(url)); 
#endif
}

bool Coastal::email(QString &topic, QString &body)
{
#ifndef Q_OS_WIN
    if(QFile::exists("/usr/bin/xdg-email")) {
        QString cmd = "xdg-email";
        QStringList args;
        args << "--subject" << topic << "--body" << body;
        return QProcess::startDetached(cmd, args);
    }
#endif

#if defined(Q_OS_MAC)
    QString url("mailto:?subject=%1&body=%2");
    QString etopic(QUrl::toPercentEncoding(topic));
    QString ebody(QUrl::toPercentEncoding(body));
    QString formattedContent(QString(url).arg(etopic).arg(ebody));
    return QDesktopServices::openUrl(QUrl(url)); 
#else
    QString url("mailto:?subject=%1&body=%2");
    QString encoded(QUrl::toPercentEncoding(body));
    QString formattedContent(QString(url).arg(topic).arg(encoded));
    return QDesktopServices::openUrl(QUrl(url)); 
#endif
}

bool Coastal::applyStyle(QString style)
{
    QFile file(style);
    if (file.exists()) {
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
        return true;
    }
    return false;
}

void Coastal::paintRect(QPainter *painter, const QRect& rect, QColor color, qreal outline, qreal xradius, qreal yradius)
{
    painter->save();
    if(outline > 0.0) {
        QPen pen(color);
        pen.setWidth(outline);
        painter->setPen(pen);
        painter->drawRoundedRect(rect, xradius, yradius);
    }
    else {
        QPainterPath path;
        path.addRoundedRect(rect, xradius, yradius);
        painter->fillPath(path, color);
    }
    painter->restore();
}

void Coastal::paintLabel(QPainter *painter, QPoint pos, unsigned size, unsigned radius, QString text, QColor badge_color, QColor text_color, QFont text_font)
{
    if(radius > size / 2)
        radius = size - 2;

    QRect rect(pos.x(), pos.y(), size, size);
    painter->save();

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    painter->fillPath(path, badge_color);

    painter->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
    size = (size - 1) * (text.size() > 1 ? .95 : .75);
    text_font.setPixelSize((int)(size / text.size()));
    painter->setFont(text_font);
    rect.adjust(0, 5, 0, 0);

    QPen text_pen(text_color);
    painter->setPen(text_pen);
    painter->drawText(rect, Qt::AlignCenter, text);
    painter->restore();
}

QString Coastal::localCache()
{
    QString org;
    QString app = QCoreApplication::applicationName();
    QString cache = QDir::homePath() + "/.cache";
    if(app.isEmpty()) {
        app = QCoreApplication::applicationFilePath();
        int pos = app.lastIndexOf('/');
        if(pos > -1)
            app = app.mid(++pos);
    }
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    org = QCoreApplication::organizationName();
    if(org.isEmpty())
        org = QCoreApplication::organizationDomain();
#endif
#ifdef Q_OS_MAC
    cache = QDir::homePath() + "/Library/Application Support";
#endif
#ifdef Q_OS_WIN
    cache = env("LOCALAPPDATA");
    if(cache.isEmpty())
        cache = env("USERPROFILE") + "/Local Settings/Application Data";
#endif

    if(org.isEmpty())
        return cache + "/" + app;
    else
        return cache + "/" + org + "/" + app;
}

QString Coastal::sharedData()
{
    QString programData = "/usr/share";
    QString org;
    QString app = QCoreApplication::applicationName();
    if(app.isEmpty()) {
        app = QCoreApplication::applicationFilePath();
        int pos = app.lastIndexOf('/');
        if(pos > -1)
            app = app.mid(++pos);
    }

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    org = QCoreApplication::organizationName();
    if(org.isEmpty())
        org = QCoreApplication::organizationDomain();
#endif
#ifdef Q_OS_WIN
    programData = env("ProgramData");
    if(programData.isEmpty())
        programData = env("ALLUSERSPROFILE");
    if(programData.isEmpty())
        programData = "C:/ProgramData";
#endif
    if(org.isEmpty())
        return programData + "/" + app;
    else
        return programData + "/" + org + "/" + app;
}

QString Coastal::documentsPath()
{
#if QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if(path.isEmpty())
        path = QDir::homePath();
    return path;
#else
    QString path = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    if(path.isEmpty() || !QDir(path).exists())
        path = QDir::homePath();
    return path;
#endif
}

QString Coastal::desktopPath()
{
#if QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if(path.isEmpty())
        path = QDir::homePath();
    return path;
#else
    QString path = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    if(path.isEmpty() || !QDir(path).exists())
        path = QDir::homePath();
    return path;
#endif
}

QString Coastal::downloadsPath()
{
#if QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if(path.isEmpty())
        path = desktopPath();
    return path;
#else
    QString path = QDir::homePath() + "/Downloads";
    if(path.isEmpty() || !QDir(path).exists()) {
        path = QDir::homePath() + "/save";
        if(path.isEmpty() || !QDir(path).exists())
            path = desktopPath();
    }
    return path;
#endif
}
