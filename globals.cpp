/**
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

#include "globals.h"

/*****************************************************************************/
// Global objects and shortcuts
Opa opa;
MainWindow * mainWindow;
OperatorWidget * editedOperators[4];
ProgramWidget * editedProgram;
int currentProgram;
int currentAlgorithm;

/*****************************************************************************/
// MIDI relatad informations
int midiBendRange;
bool midiChannelsAct[16];
bool midiSamplesAct[32];

/*****************************************************************************/
void programNameFromQS(const QString &qsName, uint8_t opaName[])
{
    QByteArray ba = qsName.toUtf8();
    memset(opaName, 0, OPA_PROGS_NAME_LEN);
    memcpy(opaName, ba.constData(), cmin(OPA_PROGS_NAME_LEN, ba.length()));
}

void programNameToQS(const uint8_t opaName[], QString &qsName)
{
    char txt[OPA_PROGS_NAME_LEN + 1];
    memcpy(txt, opaName, OPA_PROGS_NAME_LEN);
    txt[OPA_PROGS_NAME_LEN] = '\0';
    qsName = (QString) txt;
}

/*****************************************************************************/
int deltams(struct timeval t1, struct timeval t2)
{
    int ms = (t1.tv_sec - t2.tv_sec) * 1000;
    ms += (t1.tv_usec - t2.tv_usec + 500) / 1000;
    return ms;
}

void waitms(int timeout)
{
    struct timeval start, now;
    gettimeofday(&start, NULL);
    while(1) {
        gettimeofday(&now, NULL);
        if (deltams(now, start) > timeout) break;
        yield();
    }
}
