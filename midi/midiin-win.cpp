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

#include "midiin.h"

#ifdef _WIN32

#include "windef.h"
#include "mmsystem.h"

/*****************************************************************************/
int __stdcall MsgCallback(uint32_t hmi, uint32_t msg, uint32_t instance, uint32_t param1, uint32_t param2);

/*****************************************************************************/
MidiIn::MidiIn(int index, MIDIINCALLBACK callback) :
    callback(callback), handle(0)
{
    midiInOpen((LPHMIDIIN) &handle, index, (uint64_t) MsgCallback, (uint64_t)this, CALLBACK_FUNCTION);
    midiInStart((HMIDIIN) handle);
    this->callback = callback;
}

MidiIn::~MidiIn()
{
    if (handle) {
        midiInStop((HMIDIIN)handle);
        midiInClose((HMIDIIN)handle);
    }
}

/*****************************************************************************/
int MidiIn::getNoDevices()
{
    return midiInGetNumDevs();
}

const char * MidiIn::getDeviceInName(int index)
{
    static char name[32+1];
    MIDIINCAPSA caps;
    if (midiInGetDevCapsA(index, &caps, sizeof(MIDIINCAPSA)) != MMSYSERR_NOERROR)
        return NULL;
    memset(name, 0, 32+1);
    strncpy(name, caps.szPname, 32);
    return name;
}

/*****************************************************************************/
void MidiIn::threadFunction()
{
}

/*****************************************************************************/
int __stdcall MsgCallback(uint32_t hmi, uint32_t msg, uint32_t instance, uint32_t param1, uint32_t param2)
{
    MidiIn * midiIn = (MidiIn *) instance;
    if (!midiIn) return 0;
    if (msg != MIM_DATA) return 0;
    uint8_t data[4];
    data[0] = param1;
    data[1] = param1 >> 8;
    data[2] = param1 >> 16;
    data[3] = 0;
    midiIn->callback(data);
    return 0;
}

#endif
