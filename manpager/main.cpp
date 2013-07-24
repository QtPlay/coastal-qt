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
static QAction *amap[10];

QStringList Main::manpaths;
bool Main::hidden[10];

Main::Main(QWidget *parent) :
CoastalMain()
{
    ui.setupUi((QMainWindow *)this);
    status(tr("loading..."));

    indexData = NULL;

    program_name = "Coastal Manpager";
    program_about = "Coastal Manual Page Viewer";
    setWindowIcon(QIcon(":/manpager.png"));
    setWindowTitle(program_name);

    QSettings settings;
    resize(settings.value("size", QSize(760, 540)).toSize());
    ui.actionMenubar->setChecked(settings.value("menubar", true).toBool());
    ui.actionToolbar->setChecked(settings.value("toolbar", false).toBool());
    ui.actionStatus->setChecked(settings.value("stats", true).toBool());
    ui.toolBar->setVisible(ui.actionToolbar->isChecked());
    ui.statusbar->setVisible(ui.actionStatus->isChecked());
    ui.menubar->setVisible(ui.actionMenubar->isChecked());

#ifdef Q_OS_WIN
    const char *separator = ";";
    QString manpath = settings.value("manpath").toString();
    if(manpath.isEmpty())
        manpath = "C:\\tools\\man";
#else
    const char *separator = ":";
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

    ui.actionSupport->setIcon(QIcon(":/github.png"));

    searchGroup = new QActionGroup(this);
    ui.actionIndex->setActionGroup(searchGroup);
    ui.actionKeywords->setActionGroup(searchGroup);

    ui.indexView->setEnabled(false);
    ui.indexView->setShowGrid(false);
    ui.indexView->setSortingEnabled(false);
    ui.indexView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.indexView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.indexView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.indexView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui.indexView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.indexView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(open(const QPoint&)));

    // menu action signals

    connect(ui.actionOptions, SIGNAL(triggered()), this, SLOT(options()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui.actionReload, SIGNAL(triggered()), this, SLOT(reload()));
    connect(ui.actionSupport, SIGNAL(triggered()), this, SLOT(support()));

    connect(ui.actionAll, SIGNAL(triggered()), this, SLOT(all()));
    connect(ui.actionClear, SIGNAL(triggered()), this, SLOT(clear()));

    for(unsigned pos = 0; pos < 10; ++pos)
        connect(amap[pos], SIGNAL(triggered()), this, SLOT(reload()));

    // input validation

    ui.searchBox->setValidator(&index_validator);

    // forms, tabs, and view signals

    connect(ui.searchBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(search(const QString&)));
    connect(ui.searchBox, SIGNAL(activated(const QString&)), this, SLOT(load(const QString&)));
    connect(ui.tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(close(int)));
    connect(ui.indexView, SIGNAL(activated(const QModelIndex&)), this, SLOT(load(const QModelIndex&)));
    connect(ui.actionView, SIGNAL(triggered()), this, SLOT(view()));
    connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(open()));

    // application signals

    connect(this, SIGNAL(resized()), this, SLOT(columns()));
    connect(this, SIGNAL(startup()), this, SLOT(reload()), Qt::QueuedConnection);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(menu(const QPoint&)));
    connect(ui.actionToolbar, SIGNAL(toggled(bool)), ui.toolBar, SLOT(setVisible(bool)));
    connect(ui.actionStatus, SIGNAL(toggled(bool)), ui.statusbar, SLOT(setVisible(bool)));
    connect(ui.actionMenubar, SIGNAL(toggled(bool)), ui.menubar, SLOT(setVisible(bool)));

    emit startup();
}

Main::~Main()
{
#ifdef  Q_OS_WIN
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
    settings.setValue("size", size());
    settings.setValue("menubar", ui.actionMenubar->isChecked());
    settings.setValue("toolbar", ui.actionToolbar->isChecked());
    settings.setValue("status", ui.actionStatus->isChecked());

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

void Main::resizeEvent(QResizeEvent *e)
{
    emit resized();
}

void Main::status(const QString& text)
{
    Main *w = (Main *)ui.statusbar->parent();

    ui.statusbar->setStyleSheet("color: black");
    ui.statusbar->showMessage(text);
    ui.statusbar->update();
    ui.statusbar->repaint();
    ui.statusbar->update();
    w->update();
}

void Main::error(const QString& text)
{
    Main *w = (Main *)ui.statusbar->parent();

    ui.statusbar->setStyleSheet("color: red");
    ui.statusbar->showMessage(text);
    ui.statusbar->update();
    ui.statusbar->repaint();
    w->update();
}

void Main::close(int tab)
{
    View *view;

    // close of index tab actually closes all other open manpages
    if(tab == 0) {
        int count = ui.tabs->count();
        for(tab = 1; tab < count; ++tab)
            close(1);
        return;
    }

    if(Config::destroy(ui.tabs, tab))
        goto update;

    view = (View *)ui.tabs->widget(tab);
    if(!view)
        return;

    ui.tabs->removeTab(tab);
    delete view;

update:
    if(ui.tabs->count() < 2)
        ui.tabs->setTabsClosable(false);
}

void Main::search(const QString& entry)
{
    int pos;
    bool select = false;
    QString text = entry;

    int ext = entry.lastIndexOf('.');

    if(ext > 2)
        text = entry.left(ext);

    pos = indexData->find(text);

    ui.indexView->setModel(NULL);

    if(text.length() < 1) {
        status(tr("ready"));
        indexData->select(0, text);
        select = true;
    }
    else if(pos >= 0) {
        status(tr("searching ") + QChar('\"') + text + QChar('\"'));
        indexData->select(pos, text);
        select = true;
    }
    else {
        pos = -1;
        error(tr("not found ") + QChar('\"') + text + QChar('\"'));
    }

    ui.indexView->setModel(indexData);
    columns();

    if(select)
        ui.indexView->selectRow(0);
}

void Main::load(const QString& text)
{
    int ext = text.lastIndexOf('.');

    if(ext < 2) {
        error(tr("specify section in input"));
        return;
    }

    int pos = indexData->search(text);

    if (pos < 0) {
        error(tr("not found ") + QChar('\"') + text + QChar('\"'));
        return;
    }

    load(pos);
}

void Main::view(void)
{
    QModelIndex index = ui.indexView->currentIndex();
    load(index.row());
}

void Main::load(const QModelIndex& index)
{
    load(index.row());
}

void Main::open(void)
{
    QModelIndex index = ui.indexView->currentIndex();
    QString name = indexData->name(index.row());
    Index::fileinfo node = indexData->node(index.row());
    QString path = manpaths[node.path] + QDir::separator() + "man" + node.id + QDir::separator() + name;
    status(tr("opening ") + name);

    if(node.mode == Index::fileinfo::GZIP)
        path += ".gz";

    if(!Coastal::open(path))
        error(tr("failed to load ") + name);
}

void Main::load(int row)
{
    QString name = indexData->name(row);
    Index::fileinfo node = indexData->node(row);
    QString path = manpaths[node.path] + QDir::separator() + "man" + node.id + QDir::separator() + name;

    // if already loaded, select existing tab and exit...
    if(View::find(ui.tabs, name))
        return;

    status(tr("loading ") + name);

    if(node.mode == Index::fileinfo::GZIP) {
        path += ".gz";
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
        gunzip.waitForReadyRead();
        new View(ui.tabs, gunzip, name);
        gunzip.waitForFinished();
    }
    else {
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            error(tr("failed to load ") + name);
            return;
        }
        new View(ui.tabs, file, name);
        file.close();
    }
    status(tr("loaded ") + name);
}

void Main::clear(void)
{
    for(unsigned pos = 0; pos < 10; ++pos)
        amap[pos]->setChecked(false);

    reload();
}

void Main::all(void)
{
    for(unsigned pos = 0; pos < 10; ++pos)
        amap[pos]->setChecked(true);

    reload();
}

void Main::reload(void)
{
    ui.searchBox->clear();
    ui.searchBox->setEnabled(false);
    status(tr("loading..."));
    CoastalBusy busy;

    for(unsigned pos = 0; pos < 10; ++pos)
        hidden[pos] = !amap[pos]->isChecked();

    ui.indexView->setModel(NULL);
    if(indexData) {
        disconnect(indexData, SIGNAL(selected(const QString&)), ui.searchBox, SLOT(setEditText(const QString&)));
        delete indexData;
    }

    indexData = new Index(ui.indexView);
    ui.indexView->setModel(indexData);
    connect(indexData, SIGNAL(selected(const QString&)), ui.searchBox, SLOT(setEditText(const QString&)));

    columns();
    ui.indexView->setEnabled(true);
    ui.indexView->selectRow(0);
    ui.searchBox->setEnabled(true);
    ui.searchBox->setFocus();
    ui.searchBox->repaint();
    status(tr("ready"));
}

void Main::open(const QPoint& pos)
{
    QMenu m;
    m.addAction(ui.actionView);
    m.addAction(ui.actionOpen);
    m.exec(ui.indexView->mapToGlobal(pos));
}

void Main::options(void)
{
    Config::create(ui.tabs, this);
}

void Main::menu(const QPoint& pos)
{
    QMenu m;
    m.addAction(ui.actionAbout);
    m.addAction(ui.actionSupport);

    m.addSeparator();
    QMenu *sections = m.addMenu(tr("Sections"));
    sections->addAction(ui.actionSection1);
    sections->addAction(ui.actionSection2);
    sections->addAction(ui.actionSection3);
    sections->addAction(ui.actionSection4);
    sections->addAction(ui.actionSection5);
    sections->addAction(ui.actionSection6);
    sections->addAction(ui.actionSection7);
    sections->addAction(ui.actionSection8);
    sections->addAction(ui.actionSectionl);
    sections->addAction(ui.actionSectionn);
    QMenu *view = m.addMenu(tr("View"));
    view->addAction(ui.actionMenubar);
    view->addAction(ui.actionToolbar);
    view->addAction(ui.actionStatus);

    m.addSeparator();
    m.addAction(ui.actionReload);
    m.addAction(ui.actionQuit);
    m.exec(mapToGlobal(pos));
}

void Main::columns(void)
{
    int size = ui.indexView->width() - ui.indexView->columnWidth(1) - 1;
    ui.indexView->setColumnWidth(0, size);
    ui.indexView->horizontalHeader()->resizeSection(0, size);
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

    Coastal::icons();

/*
    if(!QIcon::hasThemeIcon("view-refresh"))
        QIcon::setThemeName("coastal");
*/

    Main w;
    w.show();
    return app.exec();
}

