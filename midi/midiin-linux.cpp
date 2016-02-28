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

#if defined(__unix__) || defined(__unix)

/*****************************************************************************/
#include <string.h>
#include <alsa/asoundlib.h>
#include <pthread.h>

/*****************************************************************************/
int alsaRawMidiInDevicesCount();
void alsaRawMidiInDeviceName(int index, char * name, int length);
void alsaRawMidiInDevicePath(int index, char * path);
void * alsaRawMidiInListener(void * data);

/*****************************************************************************/
MidiIn::MidiIn(int index, MIDIINCALLBACK callback) :
    callback(callback), handle(0),
    threadHandle(0), threadRun(false),
    msgIndex(0), msgLength(255)
{
    char path[64];
    alsaRawMidiInDevicePath(index, path);
    if (snd_rawmidi_open((snd_rawmidi_t **) &handle, NULL, path, SND_RAWMIDI_NONBLOCK) < 0) return;
    threadRun = true;
    pthread_create((pthread_t *) &threadHandle, NULL, alsaRawMidiInListener, this);
}

MidiIn::~MidiIn()
{
    if (!handle) return;
    snd_rawmidi_close((snd_rawmidi_t *) handle);
    if (!threadRun) return;
    threadRun = false;
    pthread_join((pthread_t) threadHandle, NULL);
}

/*****************************************************************************/
int MidiIn::getNoDevices()
{
    return alsaRawMidiInDevicesCount();
}

const char * MidiIn::getDeviceInName(int index)
{
    static char name[128+1];
    alsaRawMidiInDeviceName(index, name, 128+1);
    return name;
}


/*****************************************************************************/
const uint8_t msgSize[] = {3, 3, 3, 3, 2, 2, 3, 255};
void MidiIn::threadFunction()
{
    while (threadRun) {
        uint8_t byte;
        while(snd_rawmidi_read((snd_rawmidi_t *) handle, &byte, 1) == 1) {
            if (byte & 0x80) {
                msgIndex = 0;
                msgLength = msgSize[(byte & 0x7) >> 4];
            }
            msg[msgIndex++] = byte;
            if (msgIndex > 3) msgIndex = 3;
            if (msgIndex == msgLength) {
                callback(msg);
                msgIndex = 1;
            }
        }
        pthread_yield();
    }
}

/*****************************************************************************/
void * alsaRawMidiInListener(void * data)
{
    MidiIn * midiIn = (MidiIn *) data;
    if (!midiIn) return NULL;
    midiIn->threadFunction();
    pthread_exit(NULL);
}

/*****************************************************************************/
int alsaRawMidiInDevicesCount()
{
    int noDevices = 0;
    int soundCard = -1;
    while (1) {
    //Get a sound card
        if (snd_card_next(&soundCard) < 0) break;
        if (soundCard == -1) break;
    //Scan the sound devices
        char dev[32];
        sprintf(dev, "hw:%d", soundCard);
        snd_ctl_t * ctl;
        if (snd_ctl_open(&ctl, dev, 0) >= 0) {
            int soundDevice = -1;
            while (1) {
            //Get a sound device
                if (snd_ctl_rawmidi_next_device(ctl, &soundDevice) < 0) break;
                if (soundDevice == -1) break;
                snd_rawmidi_info_t * subDeviceInfo;
                snd_rawmidi_info_alloca(&subDeviceInfo);
                snd_rawmidi_info_set_device(subDeviceInfo, soundDevice);
                snd_rawmidi_info_set_subdevice(subDeviceInfo, 0);
                snd_rawmidi_info_set_stream(subDeviceInfo, SND_RAWMIDI_STREAM_INPUT);
            //Scan the sub-devices
                snd_ctl_rawmidi_info(ctl, subDeviceInfo);
                noDevices += snd_rawmidi_info_get_subdevices_count(subDeviceInfo);
            }
            snd_ctl_close(ctl);
        }
    }
    snd_config_update_free_global();
    return noDevices;
}


void alsaRawMidiInDeviceName(int index, char * name, int length)
{
    int noDevices = 0;
    int soundCard = -1;
    name[0] = '\0';
    while (1) {
    //Get a sound card
        if (snd_card_next(&soundCard) < 0) break;
        if (soundCard == -1) break;
    //Scan the sound devices
        char dev[32];
        sprintf(dev, "hw:%d", soundCard);
        snd_ctl_t * ctl;
        if (snd_ctl_open(&ctl, dev, 0) >= 0) {
            int soundDevice = -1;
            while (1) {
            //Get a sound device
                if (snd_ctl_rawmidi_next_device(ctl, &soundDevice) < 0) break;
                if (soundDevice == -1) break;
                snd_rawmidi_info_t * subDeviceInfo;
                snd_rawmidi_info_alloca(&subDeviceInfo);
                snd_rawmidi_info_set_device(subDeviceInfo, soundDevice);
                snd_rawmidi_info_set_subdevice(subDeviceInfo, 0);
                snd_rawmidi_info_set_stream(subDeviceInfo, SND_RAWMIDI_STREAM_INPUT);
            //Scan the sub-devices
                snd_ctl_rawmidi_info(ctl, subDeviceInfo);
                int noSubDevices = snd_rawmidi_info_get_subdevices_count(subDeviceInfo);
                for (int subDevice = 0; subDevice < noSubDevices; subDevice++) {
                    if (index == noDevices) {
                        char subdev[64];
                        sprintf(subdev, "hw:%d,%d,%d", soundCard, soundDevice, subDevice);
                        snd_rawmidi_info_set_subdevice(subDeviceInfo, subDevice);
                        snd_ctl_rawmidi_info(ctl, subDeviceInfo);
                        strncpy(name, snd_rawmidi_info_get_subdevice_name(subDeviceInfo), length);
                        name[length-1] = '\0';
                        return;
                    }
                    noDevices ++;
                }
            }
            snd_ctl_close(ctl);
        }
    }
    snd_config_update_free_global();
}

void alsaRawMidiInDevicePath(int index, char * path)
{
    int noDevices = 0;
    int soundCard = -1;
    path[0] = '\0';
    while (1) {
    //Get a sound card
        if (snd_card_next(&soundCard) < 0) break;
        if (soundCard == -1) break;
    //Scan the sound devices
        char dev[32];
        sprintf(dev, "hw:%d", soundCard);
        snd_ctl_t * ctl;
        if (snd_ctl_open(&ctl, dev, 0) >= 0) {
            int soundDevice = -1;
            while (1) {
            //Get a sound device
                if (snd_ctl_rawmidi_next_device(ctl, &soundDevice) < 0) break;
                if (soundDevice == -1) break;
                snd_rawmidi_info_t * subDeviceInfo;
                snd_rawmidi_info_alloca(&subDeviceInfo);
                snd_rawmidi_info_set_device(subDeviceInfo, soundDevice);
                snd_rawmidi_info_set_subdevice(subDeviceInfo, 0);
                snd_rawmidi_info_set_stream(subDeviceInfo, SND_RAWMIDI_STREAM_INPUT);
            //Scan the sub-devices
                snd_ctl_rawmidi_info(ctl, subDeviceInfo);
                int noSubDevices = snd_rawmidi_info_get_subdevices_count(subDeviceInfo);
                for (int subDevice = 0; subDevice < noSubDevices; subDevice++) {
                    if (index == noDevices) {
                        sprintf(path, "hw:%d,%d,%d", soundCard, soundDevice, subDevice);
                        return;
                    }
                    noDevices ++;
                }
            }
            snd_ctl_close(ctl);
        }
    }
    snd_config_update_free_global();
}

#endif
