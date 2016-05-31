/**
    OPA Editor: MidiIn class
    MIDI input class - MACOS X support (CoreAudio)

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

#if defined(__APPLE__) && defined(__MACH__)

/*****************************************************************************/
#include <string.h>
#include <pthread.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreMIDI/MIDIServices.h>

/*****************************************************************************/
MIDIClientRef midiInClient = NULL;
void coreMIDIListener(const MIDIPacketList *packetList, void *readProcRefCon, void *srcConnRefCon);

/*****************************************************************************/
MidiIn::MidiIn(int index, MIDIINCALLBACK callback) :
    callback(callback), handle(0)
{
    if (!midiInClient)
        MIDIClientCreate(CFSTR("OPA Editor"), NULL, NULL, &midiInClient);
    MIDIPortRef port;
    MIDIInputPortCreate(midiInClient, CFSTR("input"), coreMIDIListener, this, &port);
    fflush(stdout);
    MIDIPortConnectSource(port, MIDIGetSource(index), (void *) index);
    handle = (uint64_t) port;
}

MidiIn::~MidiIn()
{
    if (!handle) return;
    MIDIPortDispose((MIDIPortRef) handle);
}

/*****************************************************************************/
int MidiIn::getNoDevices()
{
    return MIDIGetNumberOfSources();
}

const char * MidiIn::getDeviceInName(int index)
{
    static char name[128+1];
    MIDIEndpointRef sourceRef = MIDIGetSource(index);
    CFStringRef displayName;
    MIDIObjectGetStringProperty(sourceRef, kMIDIPropertyDisplayName, &displayName);
    CFStringGetCString(displayName, name, 129, kCFStringEncodingUTF8);
    name[128] = 0;
    return name;
}

/*****************************************************************************/
void MidiIn::threadFunction()
{

}

/*****************************************************************************/
const uint8_t msgSize[] = {3, 3, 3, 3, 2, 2, 3, 255};
void coreMIDIListener(const MIDIPacketList *packetList, void *readProcRefCon, void *srcConnRefCon)
{
    MidiIn * midiIn = (MidiIn *) readProcRefCon;
    if (!midiIn) return;
    const MIDIPacket * packet = &packetList->packet[0];
    for (unsigned int p = 0; p < packetList->numPackets; p++) {
        int msgStatus = packet->data[0];
        if (msgStatus & 0x80) {
            int msgLen = msgSize[(msgStatus & 0x7) >> 4];
            if (packet->length == msgLen) midiIn->callback((uint8_t *) packet->data);
        }
        packet = MIDIPacketNext(packet);
    }
}

/*****************************************************************************/
/* Junk yard */
/*
int coreMIDIInDevicesCount()
{
    int noDevices = 0;
    int device = 0;
    while (1) {
    //Get a device
        MIDIDeviceRef deviceRef = MIDIGetDevice(device++);
        if (!deviceRef) break;
    //Scan the sources
        int noEntities = MIDIDeviceGetNumberOfEntities(deviceRef);
        for (int entity = 0; entity < noEntities; entity++) {
            MIDIEntityRef entityRef = MIDIDeviceGetEntity(deviceRef, entity);
            noDevices += MIDIEntityGetNumberOfSources(entityRef);
        }
    }
    return noDevices;

}
*/
/*
void coreMIDIInDeviceName(int index, char * name, int length)
{
    int noDevices = 0;
    int device = 0;
    while (1) {
    //Get a device
        MIDIDeviceRef deviceRef = MIDIGetDevice(device++);
        if (!deviceRef) break;
    //Scan the sources
        int noEntities = MIDIDeviceGetNumberOfEntities(deviceRef);
        for (int entity = 0; entity < noEntities; entity++) {
            MIDIEntityRef entityRef = MIDIDeviceGetEntity(deviceRef, entity);
            int noSources = MIDIEntityGetNumberOfSources(entityRef);
            for (int source = 0; source < noSources; source++) {
                if (index == noDevices) {
                    CFStringRef displayName;
                    MIDIEndpointRef endRef = MIDIEntityGetSource(entityRef, source);
                    MIDIObjectGetStringProperty(endRef, kMIDIPropertyDisplayName, &displayName);
                    CFStringGetCString(displayName, name, length, kCFStringEncodingUTF8);
                    name[length] = 0;
                    return;
                }
                noDevices ++;
            }
        }
    }
}
*/

#endif
