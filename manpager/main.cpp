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

#include "program.h"
#include "ui_main.h"

static Ui::MainWindow ui;
static QStringList sections = QStringList() << "man1" << "man2" << "man3" << "man4" << "man5" << "man6" << "man7" << "man8" << "manl" << "mann";
static QAction *amap[10];
static const char *cmap = "12345678ln";

Main::Main(QWidget *parent) :
CoastalMain()
{
    ui.setupUi((QMainWindow *)this);
    status(tr("loading..."));

    program_name = "Coastal Manpager";
    program_about = "Coastal Manual Page Viewer";
    setWindowIcon(QIcon(":/manpager.png"));
    setWindowTitle(program_name);

    QSettings settings;
    const char *separator = ":";
#ifdef WIN32
    QString manpath = settings.value("manpath").toString();
    if(manpath.isEmpty())
        manpath = "C:\\tools\\man";
#else
    QString manpath = getenv("MANPATH");
    if(manpath.isEmpty())
        manpath = settings.value("manpath").toString();

    if(manpath.isEmpty())
        manpath = "/usr/share/man:/usr/local/share/man";
#endif

    manpaths = manpath.split(separator, QString::SkipEmptyParts);

    amap[0] = ui.actionSection1;
    amap[1] = ui.actionSection2;
    amap[2] = ui.actionSection3;
    amap[3] = ui.actionSection4;
    amap[4] = ui.actionSection5;
    amap[5] = ui.actionSection6;
    amap[6] = ui.actionSection7;
    amap[7] = ui.actionSection8;
    amap[8] = ui.actionSectionl;
    amap[9] = ui.actionSectionn;

    settings.beginGroup("Sections");
    ui.actionSection1->setChecked(settings.value("1", ui.actionSection1->isChecked()).toBool());
    ui.actionSection2->setChecked(settings.value("2", ui.actionSection2->isChecked()).toBool());
    ui.actionSection3->setChecked(settings.value("3", ui.actionSection3->isChecked()).toBool());
    ui.actionSection4->setChecked(settings.value("4", ui.actionSection4->isChecked()).toBool());
    ui.actionSection5->setChecked(settings.value("5", ui.actionSection5->isChecked()).toBool());
    ui.actionSection6->setChecked(settings.value("6", ui.actionSection6->isChecked()).toBool());
    ui.actionSection7->setChecked(settings.value("7", ui.actionSection7->isChecked()).toBool());
    ui.actionSection8->setChecked(settings.value("8", ui.actionSection8->isChecked()).toBool());
    ui.actionSectionl->setChecked(settings.value("l", ui.actionSectionl->isChecked()).toBool());
    ui.actionSectionn->setChecked(settings.value("n", ui.actionSectionn->isChecked()).toBool());
    settings.endGroup();

    ui.actionQuit->setIcon(QIcon::fromTheme("exit"));
    ui.actionReload->setIcon(QIcon::fromTheme("reload"));
    ui.actionAbout->setIcon(QIcon::fromTheme("help-about"));

    searchGroup = new QActionGroup(this);
    ui.actionIndex->setActionGroup(searchGroup);
    ui.actionKeywords->setActionGroup(searchGroup);

    selectGroup = new QActionGroup(this);
    ui.actionAll->setActionGroup(selectGroup);
    ui.actionBut->setActionGroup(selectGroup);
    ui.actionOnly->setActionGroup(selectGroup);

    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui.actionReload, SIGNAL(triggered()), this, SLOT(reload()));
    connect(this, SIGNAL(startup()), this, SLOT(reload()), Qt::QueuedConnection);

    connect(ui.tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(close(int)));
    connect(ui.indexTable, SIGNAL(cellClicked(int,int)), this, SLOT(load(int,int)));

    emit startup();
}

Main::~Main()
{
#ifdef  WIN32
    QString sep = ";";
#else
    QString sep = ":";
#endif

    QSettings settings;
    QString manpath = "";
    unsigned count = manpaths.size();
    unsigned pos = 0;

    while(pos < count) {
        if(pos)
            manpath += sep + manpaths[pos++];
        else
            manpath = manpaths[pos++];
    }

    settings.setValue("manpath", manpath);

    settings.beginGroup("Sections");
    settings.setValue("1", ui.actionSection1->isChecked());
    settings.setValue("2", ui.actionSection2->isChecked());
    settings.setValue("3", ui.actionSection3->isChecked());
    settings.setValue("4", ui.actionSection4->isChecked());
    settings.setValue("5", ui.actionSection5->isChecked());
    settings.setValue("6", ui.actionSection6->isChecked());
    settings.setValue("7", ui.actionSection7->isChecked());
    settings.setValue("8", ui.actionSection8->isChecked());
    settings.setValue("l", ui.actionSectionl->isChecked());
    settings.setValue("n", ui.actionSectionn->isChecked());
    settings.endGroup();
}

void Main::status(const QString& text)
{
    ui.statusbar->setStyleSheet("color: black");
    ui.statusbar->showMessage(text);
    ui.statusbar->update();
    ui.statusbar->repaint();
}

void Main::error(const QString& text)
{
    ui.statusbar->setStyleSheet("color: red");
    ui.statusbar->showMessage(text);
    ui.statusbar->update();
    ui.statusbar->repaint();
}

void Main::close(int tab)
{
    // close of index tab actually closes all other open manpages
    if(tab == 0) {
        int count = ui.tabs->count();
        for(tab = 1; tab < count; ++tab)
            close(1);
        return;
    }

    View *view = (View *)ui.tabs->widget(tab);
    if(!view)
        return;

    ui.tabs->removeTab(tab);
    delete view;

    if(ui.tabs->count() < 2)
        ui.tabs->setTabsClosable(false);
}

void Main::load(int row, int col)
{
    View *view;
    Index::NameItem *item = (Index::NameItem*)ui.indexTable->item(row, 1);
    Index::SectionItem *section = (Index::SectionItem *)ui.indexTable->item(row, 0);
    QString name = item->text() + "." + section->text();
    QString path = manpaths[item->pathid] + "/man" + item->secid + "/" + name;
    qDebug() << "selected " << item->text();
    qDebug() << "file path " << path;

    // if already loaded, select existing tab and exit...
    if(View::find(ui.tabs, name))
        return;

    status(tr("loading ") + name);

    if(item->fmode == Index::GZIP) {
        path += ".gz";
        qDebug() << "file path " << path;
        QString cmd = "gunzip";
        QStringList args;
        args << "-c" << path;
        QProcess gunzip(this);
        gunzip.start(cmd, args);
        gunzip.setReadChannel(QProcess::StandardOutput);
        if(!gunzip.waitForStarted()) {
            error(tr("failed to load ") + name);
            return;
        }
        qDebug() << "waiting...";
        gunzip.waitForReadyRead();
        qDebug() << "loading text";
        view = new View(ui.tabs, gunzip, name);
        gunzip.waitForFinished();
    }
    else {
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            error(tr("failed to load ") + name);
            return;
        }
        view = new View(ui.tabs, file, name);
        file.close();
    }
}

void Main::reload(void)
{
    ui.searchBox->setEnabled(false);
    status(tr("loading..."));

    Index::set(ui.indexTable);
    ui.indexTable->setEnabled(false);
    ui.indexTable->setShowGrid(false);

    for(int section = 0; section < 10; ++section) {
        update();
        status(tr("loading ") + cmap[section] + "...");
        for(int path = 0; path < manpaths.size(); ++path) {
            QDir dir(manpaths[path] + "/" + sections[section]);
            if(!dir.exists())
                continue;
            Index::add(dir, cmap[section], (unsigned)path);
        }
    }

    ui.indexTable->setEnabled(true);
    ui.searchBox->setEnabled(true);
    ui.searchBox->setFocus();
    ui.searchBox->repaint();
    status(tr("ready"));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("GNU Telephony");
    QCoreApplication::setOrganizationDomain("gnutelephony.org");
    QCoreApplication::setApplicationName("coastal-manpager");

    QTranslator translator;
    translator.load(QLocale::system().name(), TRANSLATIONS);
    app.installTranslator(&translator);

    if(!QIcon::hasThemeIcon("reload"))
        QIcon::setThemeName("coastal");

    Main w;
    w.show();
    return app.exec();
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
