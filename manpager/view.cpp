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

#ifdef  WIN32
#define strnicmp    strncasecmp
#endif

static char *skip(char *text)
{
    while(*text && !isspace(*text))
        ++text;
    return text;
}

static char *get(char *text, char **result = NULL)
{
    char *out;

    if(result)
        *result = NULL;

    if(*text == '.') {
        while(*text && !isspace(*text))
            ++text;
    }

    while(*text && isspace(*text))
        ++text;

    out = text;
    if(*text == '\"') {
        out = ++text;
        while(out && *out != '\"')
            ++out;
    }
    else while(!isspace(*out))
        ++out;

    *(out++) = 0;

    while(isspace(*out))
        ++out;

    if(result && out && *out)
        *result = out;

    return text;
}

View::View(QTabWidget *tabs, QIODevice& input, QString& title) :
QTextEdit()
{
    char buf[512];
    qint64 len;
    QString text;
    int count;
    char *body, *next, *tail;
    unsigned indent = 0;
    bool bold, underline, italic;
    bool next_bold, next_underline, next_italic;
    bool blank;

    for(;;) {
        len = input.readLine(buf, (qint64)sizeof(buf));
        if(len < 1)
            break;

        next = NULL;
        body = NULL;
        bold = underline = italic = false;
        next_bold = next_underline = next_italic = false;

        if(buf[0] != '\n')
            blank = false;

        // skip comments...
        if(!strncmp(buf, ".\\", 3))
            continue;

        if(!strncasecmp(buf, ".TH ", 4))
            text = text + "<h1 align=\"center\">" + get(buf) + "</h1>\n\n";
        else if(!strncasecmp(buf, ".SH ", 4)) {
            while(indent--)
                text = text + "</p></blockquote>\n";
            text = text + "<h2>" + get(buf) + "</h2><blockquote><p>\n";
            indent = 1;
        }
        else if(!strncasecmp(buf, ".pp", 3))
            text = text + "</p><p>\n";
        else if(!strncasecmp(buf, ".br ", 4)) {
            body = get(buf, &next);
            bold = true;
            text = text + "<b>";
        }
        else if(!strncasecmp(buf, ".br", 3))
            text = text + "<br>\n";
        else if(!strncasecmp(buf, ".B ", 3)) {
            body = skip(buf);
            bold = true;
            text = text + "<b>";
        }
        else {
            if(buf[0] != '.')
                body = buf;
        }

        if(body && *body == '\n' && indent && !blank) {
            text = text + "</p><p>";
            blank = true;
            continue;
        }

        if(!indent)
            continue;

body:
        if(!body)
            continue;

        tail = body + strlen(body);
        while(tail > body && isspace(*(--tail)))
            *tail = 0;

        blank = false;
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

        if(!next)
            text = text + "\n";

        if(next) {
            body = next;
            next = NULL;
            if(next_italic) {
                italic = true;
                text = text + " <i>";
                goto body;
            }
            if(next_underline) {
                underline = true;
                text = text + " <u>";
                goto body;
            }

            if(next_bold) {
                bold = true;
                text = text + " <b>";
                goto body;
            }

            text = text + " ";
            goto body;
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
