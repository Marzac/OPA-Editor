﻿/**
    OPA Editor: Globals
    Singleton objects and global definitions

    The MIT License (MIT)

    Source code copyright (c) 2013-2016 Frédéric Meslin
    Email: fredericmeslin@hotmail.com
    Website: www.fredslab.net
    Twitter: @marzacdev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#ifndef GLOBALS_H
#define GLOBALS_H

/*****************************************************************************/
#include "opa.h"
#include "operatorwidget.h"
#include "programwidget.h"
#include "mainwindow.h"

#include <QString>
#include <sys/time.h>

/*****************************************************************************/
extern Opa opa;
extern MainWindow * mainWindow;
extern OperatorWidget * editedOperators[4];
extern ProgramWidget * editedProgram;

extern int midiBendRange;
extern bool midiChannelsAct[16];
extern bool midiSamplesAct[32];

extern int currentProgram;
extern int currentAlgorithm;

/*****************************************************************************/
#define cmin(a, b) ((a) < (b) ? (a) : (b))
#define cmax(a, b) ((a) > (b) ? (a) : (b))

/*****************************************************************************/
void programNameFromQS(const QString &qsName, uint8_t opaName[]);
void programNameToQS(const uint8_t opaName[], QString &qsName);

/*****************************************************************************/
int deltams(struct timeval t1, struct timeval t2);
void waitms(int timeout);

/*****************************************************************************/
/** Yield instruction **/
#ifdef __WIN32
    extern "C" {
        void __stdcall Sleep(uint32_t dwMilliseconds);
    }
    #define yield() Sleep(0)
#elif defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))
    #include <sched.h>
    #define yield() sched_yield()
#endif

#endif // GLOBALS_H

