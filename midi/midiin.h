/**
    OPA Editor: MidiIn class
    MIDI input class (only windows yet)

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

#ifndef MIDIIN_H
#define MIDIIN_H

#include <stdint.h>
#include <stddef.h>
 
typedef void (* MIDIINCALLBACK)(uint8_t msg[]);

class MidiIn
{
public:
    MidiIn(int index, MIDIINCALLBACK callback = NULL);
    ~MidiIn();

    void threadFunction();

    static int getNoDevices();
    static const char * getDeviceInName(int index);

private:
    uint64_t handle;
    MIDIINCALLBACK callback;

#if defined(__unix__) || defined(__unix)
    uint64_t threadHandle;
    bool threadRun;
    int msgIndex;
    int msgLength;
    uint8_t msg[4];
#endif

};

#endif // MIDIIN_H
