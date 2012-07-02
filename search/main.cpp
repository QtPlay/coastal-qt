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
static const char *types = NULL;

Main::Main(const char *prefix) :
CoastalMain()
{
    ui.setupUi((QMainWindow *)this);
    ui.statusbar->showMessage(tr("loading..."));

    program_name = "Coastal Search";
    program_about = "Coastal File Search Utility";
    setWindowIcon(QIcon(":/search.png"));
    setWindowTitle(program_name);

    ind = NULL;

    if(prefix)
        QDir::setCurrent(prefix);

    dir.setPath(QDir::currentPath());
    history.append(dir.path());

    QSettings settings;
    resize(settings.value("size", QSize(760, 540)).toSize());
    ui.actionMenubar->setChecked(settings.value("menubar", false).toBool());
    ui.actionToolbar->setChecked(settings.value("toolbar", true).toBool());
    ui.actionStatus->setChecked(settings.value("stats", true).toBool());
    ui.menuBar->setVisible(ui.actionMenubar->isChecked());
    ui.toolBar->setVisible(ui.actionToolbar->isChecked());
    ui.statusbar->setVisible(ui.actionStatus->isChecked());

    if(types) {
        ui.filterTypes->setText(types);
        ui.filterTypes->setEnabled(false);
    }
    else {
        QString filter = settings.value("filter", ".txt;.log").toString();
        ui.filterTypes->setText(filter);

        int paths = settings.beginReadArray("paths");
    //  qDebug() << "SIZE " << paths << endl;
        for(int path = 0; path < paths; ++path) {
            settings.setArrayIndex(path);
            QString temp = settings.value("path").toString();
    //      qDebug() << "STR " << path << " VALUE " << temp << endl;
            if(temp == dir.path())
                continue;
            history.append(temp);
        }
        settings.endArray();
    }

    ui.actionQuit->setIcon(QIcon::fromTheme("exit"));
    ui.actionReload->setIcon(QIcon::fromTheme("reload"));
    ui.actionClear->setIcon(QIcon::fromTheme("editclear"));
    ui.actionAbout->setIcon(QIcon::fromTheme("help-about"));
    ui.actionSearch->setIcon(QIcon::fromTheme("search"));
    ui.actionSupport->setIcon(QIcon(":/github.png"));

    ui.indexView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.indexView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(open(const QPoint&)));

    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui.actionClear, SIGNAL(triggered()), this, SLOT(clear()));
    connect(ui.actionSearch, SIGNAL(triggered()), this, SLOT(reload()));
    connect(ui.actionSupport, SIGNAL(triggered()), this, SLOT(support()));
    connect(ui.pathButton, SIGNAL(clicked()), this, SLOT(changeDir()));
    connect(ui.pathBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectDir(int)));

    connect(ui.searchText, SIGNAL(returnPressed()), this, SLOT(reload()));
    connect(ui.searchName, SIGNAL(returnPressed()), this, SLOT(reload()));
    connect(ui.filterTypes, SIGNAL(returnPressed()), this, SLOT(reload()));

    connect(ui.tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(close(int)));
    connect(ui.indexView, SIGNAL(activated(const QModelIndex&)), this, SLOT(open(const QModelIndex&)));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(menu(const QPoint&)));
    connect(ui.actionMenubar, SIGNAL(toggled(bool)), ui.menuBar, SLOT(setVisible(bool)));
    connect(ui.actionToolbar, SIGNAL(toggled(bool)), ui.toolBar, SLOT(setVisible(bool)));
    connect(ui.actionStatus, SIGNAL(toggled(bool)), ui.statusbar, SLOT(setVisible(bool)));

    connect(ui.actionOpenTab, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui.actionOpenFile, SIGNAL(triggered()), this, SLOT(file()));

    // adding history triggers selectDir...
    ui.pathBox->addItems(history);

    clear();
}

Main::~Main()
{
    clear();
    QSettings settings;
    int pos = 0;

    settings.setValue("size", size());
    settings.setValue("menubar", ui.actionMenubar->isChecked());
    settings.setValue("toolbar", ui.actionToolbar->isChecked());
    settings.setValue("status", ui.actionStatus->isChecked());
    if(!types) {
        settings.setValue("filter", ui.filterTypes->text());

        settings.beginWriteArray("paths");
        while(pos < history.size()) {
            settings.setArrayIndex(pos);
            settings.setValue("path", history[pos++]);
        }
        settings.endArray();
    }
}

void Main::selectDir(int index)
{
    if(index < 0)
        return;

    QString path = history.takeAt(index);
    QDir::setCurrent(path);
    dir.setPath(QDir::currentPath());
//  qDebug() << "SELECT PATH " << path << " DIR " << dir.path() << endl;
    history.insert(0, dir.path());

    ui.indexView->setModel(NULL);
    if(ind) {
        delete ind;
        ind = NULL;
    }
}

void Main::changeDir(void)
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Directory"), dir.path());

    if(path.isNull() == false) {
        QDir::setCurrent(path);
        dir.setPath(QDir::currentPath());

//      qDebug() << "CHANGE PATH " << path << " DIR " << dir.path() << endl;
        unsigned pos = history.size();
        while(pos > 0) {
            --pos;
            if(dir.path() == history[pos])
                history.takeAt(pos);
        }

        // no more than 10 elements...
        while(history.size() > 9)
            history.takeLast();

        history.insert(0, dir.path());

        // triggers select dir...
        ui.pathBox->clear();
        ui.pathBox->addItems(history);

        // rest in selectDir
    }
}

void Main::clear(void)
{
    ui.indexView->setModel(NULL);
    ui.searchText->setText("");
    ui.searchName->setText("*");
    ui.searchText->setFocus();
    ui.statusbar->showMessage(tr("ready"));

    if(ind) {
        delete ind;
        ind = NULL;
    }
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

void Main::open(void)
{
    open(ui.indexView->currentIndex());
}

void Main::file(void)
{
    QModelIndex index = ui.indexView->currentIndex();
    QString name = ind->name(index.row());

    ui.statusbar->showMessage(tr("opening ") + name);
    name = ui.pathBox->currentText() + QDir::separator() + name;

    Coastal::open(name.toUtf8().constData());
}

void Main::open(const QPoint& pos)
{
    QMenu m;
    m.addAction(ui.actionOpenTab);
    m.addAction(ui.actionOpenFile);
    m.addAction(ui.actionOpenWith);

    m.exec(ui.indexView->mapToGlobal(pos));
}

void Main::open(const QModelIndex& index)
{
    QString name = ind->name(index.row());

    // select tab if already open
    if(View::find(ui.tabs, name))
        return;

    ui.statusbar->showMessage(tr("loading ") + name);
    new View(ui.tabs, name);
    ui.statusbar->showMessage(tr("loaded ") + name);
}

void Main::reload(void)
{
    ui.statusbar->showMessage(tr("reloading..."));
    ui.indexView->setModel(NULL);
    if(ind)
        delete ind;

    QString filters = ui.filterTypes->text();
    filters.replace(QChar(','), QChar(';'));
    filters.remove(QChar('*'));
    filters.remove(QChar(' '));

    ind = new Index(ui.indexView, ui.searchName->text(), filters.split(";"), ui.searchText->text());
    ui.indexView->setModel(ind);
    ui.statusbar->showMessage(tr("ready"));
}

void Main::menu(const QPoint& pos)
{
    QMenu m;

    m.addAction(ui.actionAbout);
    m.addAction(ui.actionSupport);

    m.addSeparator();
    QMenu *view = m.addMenu(tr("View"));
    view->addAction(ui.actionMenubar);
    view->addAction(ui.actionToolbar);
    view->addAction(ui.actionStatus);

    m.addSeparator();
    m.addAction(ui.actionReload);
    m.addAction(ui.actionClear);
    m.addAction(ui.actionQuit);
    m.exec(mapToGlobal(pos));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("GNU Telephony");
    QCoreApplication::setOrganizationDomain("gnutelephony.org");
    QCoreApplication::setApplicationName("coastal-search");

    QTranslator translator;
    translator.load(QLocale::system().name(), TRANSLATIONS);
    app.installTranslator(&translator);

    if(!QIcon::hasThemeIcon("reload"))
        QIcon::setThemeName("coastal");

    if(argv[1] && argv[2])
        types = argv[2];

    Main w(argv[1]);
    w.show();
    return app.exec();
}

