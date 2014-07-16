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

#include "search.h"
#include "ui_main.h"
#include "ui_toolbar.h"

static Ui::MainWindow ui;
static Ui::Toolbar tb;
static const char *types = NULL;

bool Main::casefilter = false;

Main::Main(const char *prefix) :
CoastalMain()
{
    ui.setupUi((QMainWindow *)this);
    ui.statusbar->showMessage(tr("loading..."));

    program_name = "Coastal Search";
    program_about = "Coastal File Search Utility";

    QWidget *toolbar = extendToolbar(ui.toolBar);
    tb.setupUi(toolbar);

    ind = NULL;

    if(prefix)
        QDir::setCurrent(prefix);

    dir.setPath(QDir::currentPath());
    history.append(dir.path());

    QSettings settings;
    resize(settings.value("size", QSize(760, 540)).toSize());

    if(settings.value("case", false).toBool())
        casefilter = true;

    if(settings.value("text", false).toBool())
        CoastalView::setSensitive();

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

    ui.indexView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.indexView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(open(const QPoint&)));

    connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui.actionClear, SIGNAL(triggered()), this, SLOT(clear()));
    connect(ui.actionSearch, SIGNAL(triggered()), this, SLOT(reload()));
    connect(ui.actionOptions, SIGNAL(triggered()), this, SLOT(options()));
    connect(ui.actionSupport, SIGNAL(triggered()), this, SLOT(support()));
    connect(ui.pathButton, SIGNAL(clicked()), this, SLOT(changeDir()));
    connect(ui.pathBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectDir(int)));

    connect(tb.searchText, SIGNAL(returnPressed()), this, SLOT(reload()));
    connect(ui.searchName, SIGNAL(returnPressed()), this, SLOT(reload()));
    connect(ui.filterTypes, SIGNAL(returnPressed()), this, SLOT(reload()));

    connect(ui.tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(close(int)));
    connect(ui.indexView, SIGNAL(activated(const QModelIndex&)), this, SLOT(open(const QModelIndex&)));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(menu(const QPoint&)));

    connect(ui.actionView, SIGNAL(triggered()), this, SLOT(view()));
    connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(open()));

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

    if(casefilter)
        settings.setValue("case", true);
    else
        settings.setValue("case", false);

    if(CoastalView::sensitive())
        settings.setValue("text", true);
    else
        settings.setValue("text", false);

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

        unsigned pos = history.size();
        while(pos > 0) {
            --pos;

            if(dir.path() == history[pos])
                history.takeAt(pos);
            else if(!QDir(history[pos]).exists())
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
    tb.searchText->setText("");
    ui.searchName->setText("*");
    tb.searchText->setFocus();
    ui.statusbar->showMessage(tr("ready"));

    if(ind) {
        delete ind;
        ind = NULL;
    }
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

void Main::view(void)
{
    open(ui.indexView->currentIndex());
}

void Main::open(void)
{
    QModelIndex index = ui.indexView->currentIndex();
    QString name = ind->name(index.row());

    ui.statusbar->showMessage(tr("opening ") + name);
    name = ui.pathBox->currentText() + QDir::separator() + name;

    Coastal::open(name);
}

void Main::open(const QPoint& pos)
{
    QMenu m;
    m.addAction(ui.actionView);
    m.addAction(ui.actionOpen);
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

void Main::options(void)
{
    Config::create(ui.tabs);
}

void Main::reload(void)
{
    CoastalBusy busy;

    ui.statusbar->showMessage(tr("searching..."));
    ui.statusbar->update();
    qApp->processEvents();
    ui.indexView->setModel(NULL);
    if(ind)
        delete ind;

    QString filters = ui.filterTypes->text();
    filters.replace(QChar(','), QChar(';'));
    filters.remove(QChar('*'));
    filters.remove(QChar(' '));

    ind = new Index(ui.indexView, ui.searchName->text(), filters.split(";"), tb.searchText->text());
    ui.indexView->setModel(ind);
    ui.statusbar->showMessage(tr("ready"));
}

void Main::menu(const QPoint& pos)
{
    QMenu m;

    m.addAction(ui.actionAbout);
    m.addAction(ui.actionOptions);
    m.addAction(ui.actionSupport);
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

    Q_INIT_RESOURCE(search);
#ifdef Q_OS_WIN
    Coastal::applyStyle(":/qss/search.css");
#else  // let others optionally style our apps from common dir...
    if(!Coastal::applyStyle("/usr/share/coastal/search.css"))
        Coastal::applyStyle(":/qss/search.css");
#endif

    if(argv[1] && argv[2])
        types = argv[2];

    Main w(argv[1]);
    w.show();
    return app.exec();
}


