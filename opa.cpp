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
    0xFF,               // volume
    0x00,               // coarse
    0x00,               // fine
    0x00,               // reserved1
    0x00,               // reserved2
    0x00,               // reserved3
    0x00,               // reserved4
    0x00,               // reserved5
};

const OpaProgramParams Opa::defaultProgram = {
    0x01,               // algorithm
    0x00,               // reserved
    0xFF,               // volume
    0x80                // panning
};

const OpaOperatorParams Opa::defaultOperator = {
    0x00,	// volume
    0x20,	// coarse
    0x00,	// fine
    0x10,	// envAttack
    0x20,	// envDecay
    0xD0,	// envSusLevel
    0x00,	// envSusTime;
    0x40,	// envRelease;
    0x20,	// LFOSpeed;
    0x00,	// LFOAmount;
    0x00,	// feedback;
    0x00,	// flags;
    0x00,   // reserved2
    0x00,   // reserved3
    0x00,   // reserved4
    0x00,   // reserved5
};

/*****************************************************************************/
Opa::Opa()
{
    paramReturn = NULL;
    programReturn = NULL;
    rxLen = 0;
    port = -1;
    state = OPA_DISCONNECTED;
    enabled = true;
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

    programReturn = NULL;
    paramReturn = 0;
    programReturnIndex = 0;
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
    if (programReturn && rxLen >= 2 + sizeof(OpaProgram)) {
        parseProgram();
        rxLen = 0;
    }

// Expected nothing
    if (!paramReturn && !programReturn)
        rxLen = 0;
}

/*****************************************************************************/
void Opa::fetchSerialData()
{
    unsigned int len = 0;
    char tmp[64];
    do {
        len = comRead(port, tmp, 64);
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
    * paramReturn = rxBuffer[3];
    paramReturn = NULL;
    paramReturnIndex = 0;
}

void Opa::parseProgram()
{
    char cmdIndex = rxBuffer[0];
    char cmdProg = rxBuffer[1];
    if (cmdIndex != OPA_PROGRAMWRITE) return;
    if (cmdProg != programReturnIndex) return;
    fprintf(stdout, "Receiving program %i\n", cmdProg);
    memcpy(programReturn, &rxBuffer[2], sizeof(OpaProgram));
    programReturn = NULL;
    programReturnIndex = 0;
}

/*****************************************************************************/
void Opa::noteOn(int instrument, int note, int expression)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Note on %i: note: %i, expression: %i\n", instrument, note, expression);
    fflush(stdout);
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_NOTEON;
    buffer[1] = instrument;
    buffer[2] = note;
    buffer[3] = expression;
// Send the message
    comWrite(port, buffer, 4);
}

void Opa::noteOff(int instrument, int note)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Note off %i: note: %i\n", instrument, note);
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
   // if (paramReturn || programReturn) return;
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
void Opa::programWrite(int program, OpaProgram * programData)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Program write %i", program);
    fflush(stdout);
// Prepare the message
    char buffer[2 + sizeof(OpaProgram)];
    buffer[0] = OPA_PROGRAMWRITE;
    buffer[1] = program;
    memcpy(&buffer[2], programData, sizeof(OpaProgram));
// Send the message
    comWrite(port, buffer, 2 + sizeof(OpaProgram));
}

void Opa::programRead(int program, OpaProgram * programData)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Program read %i", program);
    fflush(stdout);
// Store the pointer
    if (paramReturn || programReturn) return;
    programReturn = programData;
    programReturnIndex = program;
// Prepare the message
    char buffer[2];
    buffer[0] = OPA_PROGRAMREAD;
    buffer[1] = program;
// Send the message
    comWrite(port, buffer, 2);
}

/*****************************************************************************/
void Opa::programStore(int program, int slot)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Program store %i: slot %i\n", program, slot);
    fflush(stdout);
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_PROGRAMSTORE;
    buffer[1] = program;
    buffer[2] = slot;
// Send the message
    comWrite(port, buffer, 4);
}

void Opa::programLoad(int program, int slot)
{
// Check for port
    if (port < 0 || !enabled) return;
    fprintf(stdout, "Program load %i: slot %i\n", program, slot);
    fflush(stdout);
// Prepare the message
    char buffer[4];
    buffer[0] = OPA_PROGRAMLOAD;
    buffer[1] = program;
    buffer[2] = slot;
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

/*****************************************************************************/
