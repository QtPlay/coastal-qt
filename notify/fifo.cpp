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

#include "program.h"

using namespace std;

Fifo::Fifo()
{
	moveToThread(this);
}

void Fifo::process(char *buf)
{
    char *cp = strchr(buf, '\n');
    if(cp)
        *cp = 0;

    cp = strchr(buf, '\t');
    if(!cp)
        return;

    *(cp++) = 0;
    if(!strcmp(buf, "notice")) {
        QString title, summary, icon;
        char *c3 = NULL, *c2 = strchr(cp, '\t');
        if(c2) {
            *(c2++) = 0;
            c3 = strchr(c2, '\t');
            if(c3)
                *(c3++) = 0;
        }
        title = cp;
        if(c2)
            summary = c2;
        if(c3)
            icon = c3;
    
        emit notice(title, summary, icon);
    }
}

#if defined(Q_OS_WIN)

static HANDLE hFifo = INVALID_HANDLE_VALUE;
static HANDLE hLoopback = INVALID_HANDLE_VALUE;
static HANDLE hEvent = INVALID_HANDLE_VALUE;
static OVERLAPPED ovFifo;

void Fifo::run(void)
{
    static char buf[464];
    BOOL result;
    DWORD msgresult;

	hFifo = CreateMailslot("\\\\.\\mailslot\\notify_ctrl", 0, MAILSLOT_WAIT_FOREVER, NULL);
    if(hFifo == INVALID_HANDLE_VALUE)
        return;

    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    hLoopback = CreateFile(buf, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ovFifo.Offset = 0;
    ovFifo.OffsetHigh = 0;
    ovFifo.hEvent = hEvent;

    for(;;) {
        result = ReadFile(hFifo, buf, sizeof(buf) - 1, &msgresult, &ovFifo);
        if(!result && GetLastError() == ERROR_IO_PENDING) {
            int ret = WaitForSingleObject(ovFifo.hEvent, INFINITE);
            if(ret != WAIT_OBJECT_0)
                continue;
            result = GetOverlappedResult(hFifo, &ovFifo, &msgresult, TRUE);
        }   
        if(!result || msgresult < 1)
            break;
        buf[msgresult] = 0;

        process(buf);
    }
}

void Fifo::stop(void)
{
    CloseHandle(hFifo);
    CloseHandle(hLoopback);
    CloseHandle(hEvent);
    hFifo = hLoopback = hEvent = INVALID_HANDLE_VALUE;
    wait();
}

#else

#include <sys/stat.h>

static FILE *fifo = NULL;
static char fifopath[128] = "";

void Fifo::run(void)
{
    static char buf[512];

    char *env = getenv("HOME");
    if(!env)
        return;

    snprintf(fifopath, sizeof(fifopath), "%s/.coastal-notify", env);
    remove(fifopath);
    if(mkfifo(fifopath, 0600)) {
        fifopath[0] = 0;
        return;
    }

    fifo = fopen(fifopath, "r+");
    for(;;) {
        if(fgets(buf, sizeof(buf), fifo) == NULL)
            break;
        if(!strcmp(buf, "quit\n"))
            break;

        process(buf);
    }
}

void Fifo::stop(void)
{
    if(fifopath[0]) {
        FILE *fp = fopen(fifopath, "w");
        fprintf(fp, "quit\n");
        fclose(fp);
        ::remove(fifopath);
        fclose(fifo);
    }
    wait();
}

#endif
















