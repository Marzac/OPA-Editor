/**
    OPA Editor: OPA communication class
    Enable communication with the OPA Shield

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

#include "opa.h"

#include "rs232/rs232.h"
#include "globals.h"

#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

/*****************************************************************************/
const uint16_t opaComLengths[] = {
    1,      // Status
    4,      // Note on
    4,      // Note off
    2,      // All notes off
    1,      // All sounds off
    5,      // Parameter write
    3,      // Parameter read
    0,      // Program write
    2,      // Program read
    3,      // Save program
    3,      // Load program
};

/*****************************************************************************/
const OpaGlobals Opa::defaultGlobals = {
    0xFF,                   // volume
    0x00,                   // coarse
    0x00,                   // fine
    OPA_GLOBAL_DEFAULT,     // flags
    0x00,                   // reserved1
    0x00,                   // reserved2
    0x00,                   // reserved3
    0x00,                   // reserved4
};

const OpaProgramParams Opa::defaultProgramParams = {
    {'P', 'r', 'o', 'g', 'r', 'a', 'm', ' '},
    0x01,                   // algorithm
    0xFF,                   // volume
    0x80,                   // panning
    OPA_PROGRAM_DEFAULT,    // flags
};

const OpaOperatorParams Opa::defaultOperatorParams = {
    0x00,                   // volume
    0x20,                   // coarse
    0x00,                   // fine
    0x10,                   // envAttack
    0x20,                   // envDecay
    0xD0,                   // envSusLevel
    0x00,                   // envSusTime
    0x40,                   // envRelease
    0x20,                   // LFOSpeed
    0x00,                   // LFOAmount
    0x00,                   // feedback
    OPA_OP_DEFAULT,         // flags
    0x00,                   // reserved2
    0x00,                   // reserved3
    0x00,                   // reserved4
    0x00,                   // reserved5
};

/*****************************************************************************/
OpaProgram Opa::defaultProgram;

/*****************************************************************************/
Opa::Opa()
{
    initDefaultProgram();
    paramReturn = NULL;
    programReturn = NULL;
    globalsReturn = NULL;

    rxLen = 0;
    port = -1;

    state = OPA_DISCONNECTED;
    enabled = true;
}

/*****************************************************************************/
void Opa::initDefaultProgram()
{
    defaultProgram.params = defaultProgramParams;
    defaultProgram.opParams[0] = defaultOperatorParams;
    defaultProgram.opParams[1] = defaultOperatorParams;
    defaultProgram.opParams[2] = defaultOperatorParams;
    defaultProgram.opParams[3] = defaultOperatorParams;
    defaultProgram.opParams[0].volume = 0xE0;
}

/*****************************************************************************/
void Opa::connect(int port)
{
    disconnect();
    int result = comOpen(port, OPA_COM_BAUDRATE);
    if (!result) {
        state = OPA_DISCONNECTED;
        return;
    }
    this->port = port;

    globalsReturn = NULL;
    programReturn = NULL;
    programReturnIndex = 0;
    paramReturn = NULL;
    paramReturnIndex = 0;

    state = OPA_CONNECTED;
}

void Opa::disconnect()
{
    state = OPA_DISCONNECTED;
    if (port == -1) return;
    comClose(port);
    port = -1;
}

bool Opa::isConnected()
{
    return state >= OPA_CONNECTED;
}

/*****************************************************************************/
void Opa::setEnable(bool enable)
{
    enabled = enable;
}

/*****************************************************************************/
void Opa::update()
{
    if (port < 0) return;
    fetchSerialData();

// Check for parameter
    if (paramReturn && rxLen >= 4) {
        parseParameter();
        rxLen = 0;
    }

// Check for program
    if (programReturn && rxLen >= 4 + sizeof(OpaProgram)) {
        parseProgram();
        rxLen = 0;
    }

// Check for globals
    if (globalsReturn && rxLen >= 3 + sizeof(OpaGlobals)) {
        parseGlobals();
        rxLen = 0;
    }

// Expected nothing
    if (!isWaiting())
        rxLen = 0;
}

/*****************************************************************************/
void Opa::fetchSerialData()
{
    unsigned int len = 0;
    char tmp[64];
    do {
        len = comRead(port, tmp, 64);
        if (len) {
            fprintf(stdout, "Receiving data %i\n", len);
            fflush(stdout);
        }
        int cpy = cmin(len, OPA_RXBUFFER_LEN - rxLen);
        memcpy(&rxBuffer[rxLen], tmp, cpy);
        rxLen += cpy;
    }while(len);

}

/*****************************************************************************/
void Opa::parseParameter()
{
    char cmdIndex = rxBuffer[0];
    char cmdParam = rxBuffer[2];
    if (cmdIndex != OPA_PARAMWRITE) return;
    if (cmdParam != paramReturnIndex) return;
    fprintf(stdout, "Receiving param %i\n", cmdParam);
    fflush(stdout);
    * paramReturn = rxBuffer[3];
    paramReturn = NULL;
    paramReturnIndex = 0;
}

void Opa::parseProgram()
{
    char cmdIndex = rxBuffer[0];
    char cmdProg = rxBuffer[1];
    if (cmdIndex != OPA_PROGRAMWRITE &&
        cmdIndex != OPA_INTERNALWRITE) return;
    if (cmdProg != programReturnIndex) return;
    fprintf(stdout, "Receiving program %i\n", cmdProg);
    fflush(stdout);
    memcpy((void *) programReturn, &rxBuffer[4], sizeof(OpaProgram));
    programReturn = NULL;
    programReturnIndex = 0;
}

void Opa::parseGlobals()
{
    char cmdIndex = rxBuffer[0];
    if (cmdIndex != OPA_GLOBALSWRITE) return;
    fprintf(stdout, "Receiving globals\n");
    fflush(stdout);
    memcpy((void *) globalsReturn, &rxBuffer[3], sizeof(OpaGlobals));
    globalsReturn = NULL;
}

/*****************************************************************************/
void Opa::noteOn(int instrument, int note, int fraction)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Note on %i: note: %i, fraction: %i\n", instrument, note, fraction);
    fflush(stdout);
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_NOTEON;
    buffer[1] = instrument;
    buffer[2] = note;
    buffer[3] = 0;
// Send the message
    comWrite(port, buffer, 4);
}

void Opa::noteOff(int instrument, int note, int fraction)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Note off %i: note: %i, fraction: %i\n\n", instrument, note, fraction);
    fflush(stdout);
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_NOTEOFF;
    buffer[1] = instrument;
    buffer[2] = note;
    buffer[3] = 0;
// Send the message
    comWrite(port, buffer, 4);
}

void Opa::allNotesOff(int instrument)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "All notes off %i\n", instrument);
    fflush(stdout);
// Prepare the message
    char buffer[2];
    buffer[0] = OPA_ALLNOTESOFF;
    buffer[1] = instrument;
// Send the message
    comWrite(port, buffer, 2);
}

void Opa::allSoundsOff()
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "All sounds off\n");
    fflush(stdout);
// Prepare the message
    char buffer[1];
    buffer[0] = OPA_ALLSOUNDSOFF;
// Send the message
    comWrite(port, buffer, 1);
}

/*****************************************************************************/
void Opa::paramWrite(int program, int param, int value)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Program %i: writing param %i, value: %i\n", program, param, value);
    fflush(stdout);
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_PARAMWRITE;
    buffer[1] = program;
    buffer[2] = param;
    buffer[3] = value;
// Send the message
    comWrite(port, buffer, 4);
}

void Opa::paramRead(int program, int param, int * value)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Program %i: reading param %i\n", program, param);
    fflush(stdout);
// Store the pointer
    if (isWaiting()) return;
    paramReturn = value;
    paramReturnIndex = param;
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_PARAMREAD;
    buffer[1] = program;
    buffer[2] = param;
    buffer[3] = 0;
// Send the message
    comWrite(port, buffer, 4);
}

/*****************************************************************************/
void Opa::globalsParamWrite(int param, int value)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Globals: writing param %i, value: %i\n", param, value);
    fflush(stdout);
// Prepare the message
    char buffer[3];
    buffer[0] = OPA_GLOBALSPARAMWRITE;
    buffer[1] = param;
    buffer[2] = value;
// Send the message
    comWrite(port, buffer, 3);
}

void Opa::globalsParamRead(int param, int * value)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Globals: reading param %i\n", param);
    fflush(stdout);
// Store the pointer
    if (isWaiting()) return;
    paramReturn = value;
    paramReturnIndex = param;
// Prepare the message
    char buffer[3];
    buffer[0] = OPA_GLOBALSPARAMREAD;
    buffer[1] = param;
    buffer[2] = 0;
// Send the message
    comWrite(port, buffer, 3);
}

/*****************************************************************************/
void Opa::programWrite(int program, const OpaProgram * programData)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Program write %i\n", program);
    fflush(stdout);
// Prepare the message
    char buffer[4 + sizeof(OpaProgram)];
    buffer[0] = OPA_PROGRAMWRITE;
    buffer[1] = program;
    buffer[2] = sizeof(OpaProgram);
    buffer[3] = 0;
    memcpy(&buffer[4], programData, sizeof(OpaProgram));
// Send the message
    comWrite(port, buffer, 4 + sizeof(OpaProgram));
}

void Opa::programRead(int program, OpaProgram * programData)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Program read %i\n", program);
    fflush(stdout);
// Store the pointer
    if (isWaiting()) return;
    programReturn = programData;
    programReturnIndex = program;
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_PROGRAMREAD;
    buffer[1] = program;
    buffer[2] = sizeof(OpaProgram);
    buffer[3] = 0;
// Send the message
    comWrite(port, buffer, 4);
}

/*****************************************************************************/
void Opa::globalsWrite(const OpaGlobals * globalsData)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Global write\n");
    fflush(stdout);
// Prepare the message
    char buffer[3 + sizeof(OpaGlobals)];
    buffer[0] = OPA_GLOBALSWRITE;
    buffer[1] = sizeof(OpaGlobals);
    buffer[2] = 0;
    memcpy(&buffer[3], globalsData, sizeof(OpaGlobals));
// Send the message
    comWrite(port, buffer, 3 + sizeof(OpaGlobals));
}

void Opa::globalsRead(OpaGlobals * globalsData)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Global read\n");
    fflush(stdout);
// Store the pointer
    if (isWaiting()) return;
    globalsReturn = globalsData;
// Prepare the message
    char buffer[3];
    buffer[0] = OPA_GLOBALSREAD;
    buffer[1] = sizeof(OpaGlobals);
    buffer[2] = 0;
// Send the message
    comWrite(port, buffer, 3);
}

/*****************************************************************************/
void Opa::internalStore(int program, int slot)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Internal store %i: slot %i\n", program, slot);
    fflush(stdout);
// Prepare the message
    char buffer[3];
    buffer[0] = OPA_INTERNALSTORE;
    buffer[1] = program;
    buffer[2] = slot;
// Send the message
    comWrite(port, buffer, 3);
}

void Opa::internalLoad(int program, int slot)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Internal load %i: slot %i\n", program, slot);
    fflush(stdout);
// Prepare the message
    char buffer[3];
    buffer[0] = OPA_INTERNALLOAD;
    buffer[1] = program;
    buffer[2] = slot;
// Send the message
    comWrite(port, buffer, 3);
}


void Opa::internalWrite(int slot, const OpaProgram * programData)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Internal write %i", slot);
    fflush(stdout);
// Prepare the message
    char buffer[4 + sizeof(OpaProgram)];
    buffer[0] = OPA_INTERNALWRITE;
    buffer[1] = slot;
    buffer[2] = sizeof(OpaProgram);
    buffer[3] = 0;
    memcpy(&buffer[4], programData, sizeof(OpaProgram));
// Send the message
    comWrite(port, buffer, 4 + sizeof(OpaProgram));
}

void Opa::internalRead(int slot, OpaProgram * programData)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Internal read %i\n", slot);
    fflush(stdout);
// Store the pointer
    if (isWaiting()) return;
    programReturn = programData;
    programReturnIndex = slot;
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_INTERNALREAD;
    buffer[1] = slot;
    buffer[2] = sizeof(OpaProgram);
    buffer[3] = 0;
// Send the message
    comWrite(port, buffer, 4);
}

/*****************************************************************************/
void Opa::pitchBend(int program, int coarse, int fine)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Pitch-bend %i: coarse: %i, fine: %i\n", program, coarse, fine);
    fflush(stdout);
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_PITCHBEND;
    buffer[1] = program;
    buffer[2] = coarse;
    buffer[3] = fine;
// Send the message
    comWrite(port, buffer, 4);
}
