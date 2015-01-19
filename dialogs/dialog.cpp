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
#include <QTextStream>

static enum {NONE, TEXT, VIEW, ENTRY} mode = NONE;
static const char *filename = NULL;
static unsigned tabs = 8;
static unsigned hsize = 300, vsize = 0;
static unsigned spacing = 0;
static bool password = false;   // password sub-mode
static enum {ACCEPT, CANCEL, DEFAULT} focus = ACCEPT;
static QString textString;
static QString titleString;
static QString inputString;
static QString placeholderString;
static QString acceptString;
static QString cancelString;
static QString inputIcon;
static QString styleString;
static QLineEdit *edit;
static bool triggered = false;
static QLabel *prompt = NULL;
static unsigned exitResult = 1;     // default for cancel button/exit ...
static QPushButton *acceptButton = NULL;
static QPushButton *cancelButton = NULL;
static QTimer timer;
static unsigned timing = 0;

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

	if(mode == VIEW && !hsize)
		hsize = 300;

    if(vsize || hsize)
        resize(hsize, vsize);
    QVBoxLayout *spine = new QVBoxLayout(this);

    if(!titleString.isEmpty())
        window()->setWindowTitle(titleString);

	if(mode == ENTRY) {
		if(textString.isEmpty() && password)
			textString = tr("Type your password");
		else if(textString.isEmpty())
			textString = tr("Enter new text:");
		if(password && inputString.isEmpty())
			inputString = tr("Password:");

		if(password && inputIcon.isEmpty())
			inputIcon = ":/images/password.png";

		input = new QHBoxLayout();
		if(!inputString.isEmpty()) {
            prompt = new QLabel(this);
			prompt->setText(inputString);
			input->addWidget(prompt);
		}
		spine->setSpacing(16);
		edit = new QLineEdit(this);
        if(triggered)
            connect(edit, SIGNAL(textChanged(QString)), this, SLOT(editing(QString)));
		if(password)
			edit->setEchoMode(QLineEdit::Password);
		if(!placeholderString.isEmpty())
			edit->setPlaceholderText(placeholderString);
		input->addWidget(edit);
	}

    if(((mode == ENTRY) || (mode == TEXT)) && !inputIcon.isEmpty()) {
        if(!header)
            header = new QHBoxLayout();

        QLabel *icon = new QLabel(this);
        QPixmap image(inputIcon);
        icon->setPixmap(image);
        icon->setFixedSize(image.size());
        if(prompt)
            prompt->setFixedWidth(image.width());
        header->addWidget(icon);
    }
 
    if((mode == TEXT) || ((mode == ENTRY) && !textString.isEmpty())) {
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
		if(inputIcon.isEmpty())
	        text->setAlignment(Qt::AlignLeft|Qt::AlignTop|Qt::AlignJustify);
        else
            text->setAlignment(Qt::AlignVCenter|Qt::AlignVCenter|Qt::AlignJustify);
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

	if(spacing)
		spine->setSpacing(spacing);

    if(header)
        spine->addLayout(header);

    if(input)
        spine->addLayout(input);

    QHBoxLayout *buttons = new QHBoxLayout();
    spine->addLayout(buttons);

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttons->addItem(spacer);
    if(!acceptString.isEmpty()) {
        acceptButton = new QPushButton(this);
        if(focus == ACCEPT && !triggered)
            acceptButton->setFocus();
        acceptButton->setText(acceptString);
        connect(acceptButton, SIGNAL(clicked()), this, SLOT(accepted()));
        buttons->addWidget(acceptButton);
        if(triggered)
            acceptButton->setEnabled(false);
    }
    if(!cancelString.isEmpty()) {
        cancelButton = new QPushButton(this);
        if(focus == CANCEL || triggered)
            cancelButton->setFocus();
        cancelButton->setText(cancelString);
        connect(cancelButton, SIGNAL(clicked()), qApp, SLOT(quit()));
        buttons->addWidget(cancelButton);
    }
    else {
        QSpacerItem *tail = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        buttons->addItem(tail);
    }   

	if(edit) {
		edit->setFocus();
		edit->clear();
		edit->update();
	}

    show();

    if(timing) {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
        timer.start(timing * 1000l);
    }
}

void Process::editing(QString string)
{
	if(timing) {
		timer.stop();
		timer.start(timing * 1000);
	}

    if(!acceptButton)
        return;

    if(string.isEmpty())
        acceptButton->setEnabled(false);
    else
        acceptButton->setEnabled(true);
}

void Process::accepted(void)
{
    exitResult = 0;
    qApp->quit();
}

void Process::timeout(void)
{
    exitResult = 2;
    qApp->quit();
}

int Process::main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	Q_INIT_RESOURCE(dialog);
	Coastal::bind(app, "dialog");

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

		if(!strcmp(arg, "input") || !strcmp(arg, "entry") || !strcmp(arg, "password")) {
			if(mode != NONE) {
                fprintf(stderr, "*** coastal-dialog: mode already selected\n");
                return 3;
            }
			mode = ENTRY;
			focus = DEFAULT;
			if(!strcmp(arg, "password"))
				password = true;
			continue;
		}

		if(!strcmp(arg, "show")) {
            if(mode != NONE) {
                fprintf(stderr, "*** coastal-dialog: mode already selected\n");
                return 3;
            }
			cancelString = "";
			acceptString = tr("&Ok");
            mode = TEXT; 
            continue;
        }

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

		if(!strncmp(arg, "ok=", 3)) {
			cancelString = "";
			acceptString = QString(arg + 3);
			focus = ACCEPT;
			continue;
		}

		if(!strncmp(arg, "prompt=", 7)) {
			inputString = QString(arg + 7);
			continue;
		}

		if(!strncmp(arg, "placeholder=", 12)) {
			placeholderString = QString(arg + 12);
			continue;
		}

        if(!strncmp(arg, "accept=", 7)) {
            acceptString = QString(arg + 7);
            continue;
        }

		if(!strncmp(arg, "spacing=", 8)) {
			spacing = atoi(arg + 8);
            continue;
        }

		if(!strcmp(arg, "prompt")) {
			inputString = QString(*(++argv));
			continue;
		}

        if(!strcmp(arg, "triggered")) {
            triggered = true;
            continue;
        }

		if(!strcmp(arg, "placeholder")) {
			placeholderString = QString(*(++argv));
			continue;
		}

		if(!strcmp(arg, "ok")) {
			cancelString = "";
			acceptString = QString(*(++argv));
			focus = ACCEPT;
			continue;
		}

		if(!strcmp(arg, "spacing")) {
			spacing = atoi(*(++argv));
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

		if(!strncmp(arg, "style=", 6)) {
            styleString = QString(arg + 6);
            continue;
        }

		if(!strcmp(arg, "title")) {
			titleString = QString(*(++argv));
			continue;
		}

        if(!strcmp(arg, "style")) {
            styleString = QString(*(++argv));
            continue;
        }

        if(!strncmp(arg, "timeout=", 8)) {
            timing = atoi(arg + 8);
            continue;
        }

        if(!strcmp(arg, "timeout")) {
            timing = atoi(*(++argv));
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

	if(mode == NONE && !textString.isEmpty())
		mode = TEXT;

    if(mode == NONE) {
        fprintf(stderr, "*** coastal-dialog: no mode selected\n");
        return 3;
    }

    QCoreApplication::setOrganizationName("GNU Telephony");
    QCoreApplication::setOrganizationDomain("gnutelephony.org");
    QCoreApplication::setApplicationName("coastal-dialog");

	if(styleString.isEmpty()) {
#ifdef Q_OS_WIN
        Coastal::applyStyle(":/qss/dialog.css");
#else  // let others optionally style our apps from common dir...
        if(!Coastal::applyStyle("/usr/share/coastal/dialog.css"))
            Coastal::applyStyle(":/qss/dialog.css");
#endif
	}
	else
        Coastal::applyStyle(styleString);

    Process w;
    app.exec();
	if(!exitResult && edit)
		QTextStream(stdout) << edit->text() << "\n";
    return exitResult;
}

extern "C" {
    int main(int argc, char **argv) {
        return Process::main(argc, argv);
    }
}

        
