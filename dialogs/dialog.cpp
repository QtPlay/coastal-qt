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

#include "dialog.h"

static enum {NONE, TEXT, INPUT, PASSWORD} mode;
static const char *filename = NULL;
static unsigned tabs = 8;
static unsigned hsize = 300, vsize = 400;
static QString textstring;
static QString titlestring;
static QString inputstring;
static QString acceptstring;
static QString cancelstring;
static unsigned exitresult = 1;     // default for cancel button/exit ...

Viewer::Viewer() :
CoastalDialog()
{
    QApplication::setQuitOnLastWindowClosed(true);

    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("Viewer"));
    resize(hsize, vsize);
    QVBoxLayout *spine = new QVBoxLayout(this);

    if(!titlestring.isEmpty())
        window()->setWindowTitle(titlestring);

    if(mode == INPUT && textstring.isEmpty())
        textstring = tr("Enter new text:");
    else if(mode == PASSWORD && textstring.isEmpty())
        textstring = tr("Type your password");

    if(mode == PASSWORD && inputstring.isEmpty())
        inputstring = tr("Password:");

    if(mode == TEXT || !textstring.isEmpty()) {
        CoastalView *text = new CoastalView(this);
        text->setEnabled(true);
        spine->addWidget(text);

        QFont font = text->font();
        QFontMetrics metrics(font);
        text->setTabStopWidth(tabs * metrics.width(' '));

        if(!textstring.isEmpty()) {
            text->setText(textstring);
        }
        else {
            QFile file;
            QString temp;
            bool opened;

            if(filename) {
                file.setFileName(filename);
                opened = file.open(QIODevice::ReadOnly | QIODevice::Text);
            }
            else
                opened = file.open(stdin, QIODevice::ReadOnly | QIODevice::Text);
            int pos;
            if(opened) {
                while(!file.atEnd()) {
                    temp = file.readLine();
                    while((pos = temp.indexOf("\002")) > -1) {
                        temp.remove(pos, 1);
                    }
                    while((pos = temp.indexOf("\003")) > -1) {
                        temp.remove(pos, 3);
                    }
                    while((pos = temp.indexOf("\010")) > -1) {
                        temp.remove(pos, 1);
                    }
                    while((pos = temp.indexOf("\017")) > -1) {
                        temp.remove(pos, 1);
                    }
                    if(temp.endsWith( '\n' )) 
                        temp.chop(1);
                    if(temp.endsWith( '\r' )) 
                        temp.chop(1);
                    text->append(temp);
                }
            }
        }
    }

    QHBoxLayout *buttons = new QHBoxLayout();
    spine->addLayout(buttons);

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttons->addItem(spacer);
    if(!acceptstring.isEmpty()) {
        QPushButton *accept = new QPushButton(this);
        accept->setText(acceptstring);
        connect(accept, SIGNAL(clicked()), this, SLOT(accept()));
        buttons->addWidget(accept);
    }
    if(!cancelstring.isEmpty()) {
        QPushButton *cancel = new QPushButton(this);
        cancel->setText(cancelstring);
        connect(cancel, SIGNAL(clicked()), qApp, SLOT(quit()));
        buttons->addWidget(cancel);    
    }
    else {
        QSpacerItem *tail = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        buttons->addItem(tail);
    }   

    show();
}

void Viewer::accept(void)
{
    exitresult = 0;
    qApp->quit();
}

int Viewer::main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if(!argv[1]) {
        fprintf(stderr, "use: coastal-dialog mode [options]\n");
        return 2;
    }

    acceptstring = tr("&Accept");
    cancelstring = tr("&Cancel");

    while(*(++argv)) {
        char *arg = *argv;
        while(*arg == '-')
            ++arg;

        if(!strcmp(arg, "text-info") || !strcmp(arg, "text-view")) {
            if(mode != NONE) {
                fprintf(stderr, "*** coastal-dialog: mode already selected\n");
                return 3;
            }
            mode = TEXT;    
            continue;
        }

        if(!strncmp(arg, "tabs=", 5)) {
            tabs = atoi(arg + 5);
            continue;
        }

        if(!strcmp(arg, "tabs")) {
            tabs = atoi(*(++argv));
            continue;
        }

        if(!strncmp(arg, "text=", 5)) {
            textstring = QString(arg + 5);
            continue;
        }

        if(!strcmp(arg, "text")) {
            textstring = QString(*(++argv));
            continue;
        }

        if(!strncmp(arg, "accept=", 7)) {
            acceptstring = QString(arg + 7);
            continue;
        }

        if(!strcmp(arg, "accept")) {
            acceptstring = QString(*(++argv));
            continue;
        }

        if(!strncmp(arg, "cancel=", 7)) {
            cancelstring = QString(arg + 7);
            continue;
        }

        if(!strcmp(arg, "cancel")) {
            cancelstring = QString(*(++argv));
            continue;
        }

        if(!strncmp(arg, "title=", 6)) {
            titlestring = QString(arg + 6);
            continue;
        }

        if(!strcmp(arg, "title")) {
            titlestring = QString(*(++argv));
            continue;
        }

        if(!strncmp(arg, "height=", 7)) {
            vsize = atoi(arg + 7);
            continue;
        }

        if(!strcmp(arg, "height")) {
            vsize = atoi(*(++argv));
            continue;
        }

        if(!strncmp(arg, "width=", 6)) {
            hsize = atoi(arg + 6);
            continue;
        }

        if(!strcmp(arg, "width")) {
            hsize = atoi(*(++argv));
            continue;
        }

        if(!strncmp(arg, "filename=", 9)) {
            if(filename) {
                fprintf(stderr, "*** coastal-dialog: filename already specified\n");
                return 3;
            }
            filename = arg + 9;
            continue;
        } 

        if(!strcmp(arg, "filename")) {
            if(filename) {
                fprintf(stderr, "*** coastal-dialog: filename already specified\n");
                return 3;
            }
            filename = *(++argv);
            continue;
        } 

        fprintf(stderr, "*** coastal-dialog: %s: unknown option\n", *argv);
        return 5;
    }

    if(mode == NONE) {
        fprintf(stderr, "*** coastal-dialog: no mode selected\n");
        return 3;
    }

    QCoreApplication::setOrganizationName("GNU Telephony");
    QCoreApplication::setOrganizationDomain("gnutelephony.org");
    QCoreApplication::setApplicationName("coastal-dialog");

    QTranslator translator;
    translator.load(QLocale::system().name(), TRANSLATIONS);
    app.installTranslator(&translator);

    Viewer w;
    app.exec();
    return exitresult;
}

extern "C" {
    int main(int argc, char **argv) {
        return Viewer::main(argc, argv);
    }
}

        
