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
#include <cstring>
#include <cctype>

#define BOLD        0x01
#define ITALIC      0x02
#define UNDERLINE   0x04

static char *skip(char *text)
{
    while(*text && !isspace(*text))
        ++text;
    return text;
}

static void getargs(char *text, char **result)
{
    unsigned count = 0;
    char *out;

    // lower case dot commands...

    if(*text == '.') {
        while(*text && !isspace(*text)) {
            *text = tolower(*text);
            ++text;
        }
    }
    *(text++) = 0;

    while(*text && isspace(*text))
            ++text;

    if(*text == 0) {
        result[0] = NULL;
        return;
    }

    while(*text && count < 31) {
        if(*text == '\"') {
            out = ++text;
            while(*out && *out != '\"') {
                if(out[0] == '\\' && out[1] == '\"')
                    out += 2;
                else
                    ++out;
            }
            if(*out)
                *(out++) = 0;
        }
        else {
            out = text;
            while(!isspace(*out))
                ++out;
            if(*out)
                *(out++) = 0;
        }
        result[count++] = text;
        text = out;

        while(*text && isspace(*text))
            ++text;
    }
    if(*(++out) = '\n');
    out[1] = 0;
    result[count++] = out;
    result[count] = NULL;
}

View::View(QTabWidget *tabs, QIODevice& input, QString& title) :
QTextEdit()
{
    char buf[512];
    qint64 len;
    QString text;
    int count, argc;
    char *argv[32];
    unsigned format[9];
    unsigned indent = 0;
    bool blank = false;
    bool bold, underline, italic;
    char *body, *tail;

    for(;;) {
        len = input.readLine(buf, (qint64)sizeof(buf));
        if(len < 1)
            break;

        bold = underline = italic = false;
        argv[0] = argv[1] = NULL;
        memset(format, 0, sizeof(format));
        argc = 0;

        if(buf[0] != '\n')
            blank = false;
        else if(buf[0] == '\n' && indent && !blank) {
            text = text + "</p><p>";
            blank = true;
            continue;
        }

        // skip comments...
        if(!strncmp(buf, ".\\\"", 3))
            continue;

        if(*buf == '.')
            getargs(buf, argv);
        else
            argv[0] = buf;

        if(!strcmp(buf, ".th")) {
            text = text + "<h1 align=\"center\">" + argv[0] + "</h1>\n\n";
            continue;
        }
        if(!strcmp(buf, ".sh")) {
            while(indent--)
                text = text + "</p></blockquote>\n";
            text = text + "<h2>" + argv[0] + "</h2><blockquote><p>\n";
            indent = 1;
            continue;
        }

        if(!strcmp(buf, ".pp")) {
            text = text + "</p><p>\n";
            continue;
        }

        if(!strcmp(buf, ".br") && !argv[0]) {
            text = text + "<br>\n";
            continue;
        }

        if(!strcmp(buf, ".b") || !strcmp(buf, ".br"))
            format[0] = BOLD;
        else if(!strcmp(buf, ".ib")) {
            format[0] = ITALIC;
            format[1] = BOLD;
        }
        else if(!strcmp(buf, ".bi")) {
            format[0] = BOLD;
            format[1] = ITALIC;
        }
        else if(buf[0] == '.')              // if unknown, skip
            continue;

        if(!indent)
            continue;

        while((body = argv[argc]) != NULL) {
            if(format[argc] & BOLD) {
                bold = true;
                text = text + "<b>";
            }
            if(format[argc] & ITALIC) {
                text = text + "<i>";
                italic = true;
            }
            if(format[argc] & UNDERLINE) {
                text = text + "<U>";
                underline = true;
            }

            bool nl = false;

            tail = body + strlen(body);

            while(tail > body && isspace(*(--tail))) {
                if(*tail == '\n')
                    nl = true;
                *tail = 0;
            }

            while(body && *body) {
                switch(*body) {
                case '>':
                    text = text + "&gt;";
                    break;
                case '<':
                    text = text + "&lt;";
                    break;
                case '\"':
                    text = text + "&quot;";
                    break;
                case '&':
                    text = text + "&amp;";
                    break;
                default:
                    text = text + QChar(*body);
                }
                ++body;
            }
            if(italic)
                text = text + "</i>";
            if(underline)
                text = text + "</u>";
            if(bold)
                text = text + "</b>";
            bold = italic = underline = false;
            ++argc;
            if(nl)
                text = text + QChar('\n');
        }
    }
    setReadOnly(true);
    setEnabled(true);
    setHtml(text);

    int views = tabs->count();
    tabs->addTab(this, title);
    tabs->setCurrentIndex(views);
    tabs->setTabsClosable(true);
}

bool View::find(QTabWidget *tabs, QString& title)
{
    for(int tab = 1; tab < tabs->count(); ++tab) {
        if(tabs->tabText(tab) == title) {
            tabs->setCurrentIndex(tab);
            return true;
        }
    }
    return false;
}