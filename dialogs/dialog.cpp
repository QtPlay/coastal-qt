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

static enum {NONE, TEXT, VIEW, INPUT} mode = NONE;
static const char *filename = NULL;
static unsigned tabs = 8;
static unsigned hsize = 0, vsize = 0;
static bool password = false;   // password sub-mode
static enum {ACCEPT, CANCEL, DEFAULT} focus = ACCEPT;
static QString textString;
static QString titleString;
static QString inputString;
static QString acceptString;
static QString cancelString;
static QString inputIcon;
static unsigned exitResult = 1;     // default for cancel button/exit ...

Process::Process() :
CoastalDialog()
{
    QApplication::setQuitOnLastWindowClosed(true);

    QHBoxLayout *header = NULL;
    QHBoxLayout *input = NULL;

    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("Process"));

    if(mode == VIEW && !vsize)
        vsize = 400;

    if(vsize && !hsize)
        hsize = 300;
    else if(hsize && !vsize)
        vsize = 400;

    if(vsize || hsize)
        resize(hsize, vsize);
    QVBoxLayout *spine = new QVBoxLayout(this);

    if(!titleString.isEmpty())
        window()->setWindowTitle(titleString);

    if(mode == INPUT && textString.isEmpty())
        textString = tr("Enter new text:");
    else if(mode == INPUT && password && textString.isEmpty())
        textString = tr("Type your password");

    if(mode == INPUT && password && inputString.isEmpty())
        inputString = tr("Password:");

    if(mode == INPUT && password && inputIcon.isEmpty())
        inputIcon = ":/images/password.jpg";

    if(((mode == INPUT) || (mode == TEXT)) && !inputIcon.isEmpty()) {
        if(!header)
            header = new QHBoxLayout();

        QLabel *icon = new QLabel(this);
        icon->setPixmap(QPixmap(inputIcon));
        header->addWidget(icon);
    }
 
    if((mode == TEXT) || ((mode == INPUT) && !textString.isEmpty())) {
        if(!header)
            header = new QHBoxLayout();

        QLabel *text = new QLabel(this);

		if(textString.isEmpty()) {
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
                    textString += temp;
                }
			}
		}
        text->setText(textString);
        text->setAlignment(Qt::AlignLeft|Qt::AlignTop|Qt::AlignJustify);
        text->setText(textString);  
        header->addWidget(text);
    }
    else if(mode == VIEW) {
        if(!header)
            header = new QHBoxLayout();

        CoastalView *text = new CoastalView(this);
        text->setEnabled(true);
        header->addWidget(text);

        QFont font = text->font();
        QFontMetrics metrics(font);
        text->setTabStopWidth(tabs * metrics.width(' '));

        if(!textString.isEmpty()) {
            text->setText(textString);
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

    if(header)
        spine->addLayout(header);

    if(input)
        spine->addLayout(input);

    QHBoxLayout *buttons = new QHBoxLayout();
    spine->addLayout(buttons);

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttons->addItem(spacer);
    if(!acceptString.isEmpty()) {
        QPushButton *accept = new QPushButton(this);
        if(focus == ACCEPT)
            accept->setFocus();
        accept->setText(acceptString);
        connect(accept, SIGNAL(clicked()), this, SLOT(accept()));
        buttons->addWidget(accept);
    }
    if(!cancelString.isEmpty()) {
        QPushButton *cancel = new QPushButton(this);
        if(focus == CANCEL)
            cancel->setFocus();
        cancel->setText(cancelString);
        connect(cancel, SIGNAL(clicked()), qApp, SLOT(quit()));
        buttons->addWidget(cancel);    
    }
    else {
        QSpacerItem *tail = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        buttons->addItem(tail);
    }   

    show();
}

void Process::accept(void)
{
    exitResult = 0;
    qApp->quit();
}

int Process::main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if(!argv[1]) {
        fprintf(stderr, "use: coastal-dialog mode [options]\n");
        return 2;
    }

    acceptString = tr("&Accept");
    cancelString = tr("&Cancel");

    while(*(++argv)) {
        char *arg = *argv;
        while(*arg == '-')
            ++arg;

        if(!strcmp(arg, "text-info")) {
            if(mode != NONE) {
                fprintf(stderr, "*** coastal-dialog: mode already selected\n");
                return 3;
            }
            mode = TEXT; 
            continue;
        }

        if(!strcmp(arg, "text-view")) {
            if(mode != NONE) {
                fprintf(stderr, "*** coastal-dialog: mode already selected\n");
                return 3;
            }
            mode = VIEW; 
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
            textString = QString(arg + 5);
            continue;
        }

        if(!strcmp(arg, "text")) {
            textString = QString(*(++argv));
            continue;
        }

        if(!strncmp(arg, "accept=", 7)) {
            acceptString = QString(arg + 7);
            continue;
        }

        if(!strcmp(arg, "accept")) {
            acceptString = QString(*(++argv));
            continue;
        }

        if(!strncmp(arg, "cancel=", 7)) {
            cancelString = QString(arg + 7);
            continue;
        }

        if(!strcmp(arg, "cancel")) {
            cancelString = QString(*(++argv));
            continue;
        }

        if(!strncmp(arg, "title=", 6)) {
            titleString = QString(arg + 6);
            continue;
        }

        if(!strcmp(arg, "title")) {
            titleString = QString(*(++argv));
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

    Process w;
    app.exec();
    return exitResult;
}

extern "C" {
    int main(int argc, char **argv) {
        return Process::main(argc, argv);
    }
}

        
