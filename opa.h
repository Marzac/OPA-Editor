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

#ifndef OPA_H
#define OPA_H

#include <stdint.h>
#include <stddef.h>

/*****************************************************************************/
#define OPA_COM_BAUDRATE       57600
#define OPA_RXBUFFER_LEN       256

/*****************************************************************************/
typedef enum{
    OPA_DISCONNECTED,
    OPA_CONNECTED,
    OPA_ENUMERATED,
}OPA_COM_STATE;

/*****************************************************************************/
typedef enum{
    OPA_STATUS                 = 0,
    OPA_NOTEON                 = 1,
    OPA_NOTEOFF                = 2,
    OPA_ALLNOTESOFF            = 3,
    OPA_ALLSOUNDSOFF           = 4,
    OPA_PARAMWRITE             = 5,
    OPA_PARAMREAD              = 6,
    OPA_GLOBALSPARAMWRITE      = 7,
    OPA_GLOBALSPARAMREAD       = 8,
    OPA_PROGRAMWRITE           = 9,
    OPA_PROGRAMREAD            = 10,
    OPA_GLOBALSWRITE           = 11,
    OPA_GLOBALSREAD            = 12,
    OPA_INTERNALSTORE          = 13,
    OPA_INTERNALLOAD           = 14,
    OPA_INTERNALWRITE          = 15,
    OPA_INTERNALREAD           = 16,
    OPA_PITCHBEND              = 17,
}OPA_MESSAGES;

typedef enum{
    OPA_GLOBAL_VOLUME          = 0,
    OPA_GLOBAL_COARSE          = 1,
    OPA_GLOBAL_FINE            = 2,
    OPA_GLOBAL_FLAGS           = 3,
    OPA_GLOBAL_RESERVED1       = 4,
    OPA_GLOBAL_RESERVED2       = 5,
    OPA_GLOBAL_RESERVED3       = 6,
    OPA_GLOBAL_RESERVED4       = 7,
}OPA_GLOBAL_PARAMS;

typedef enum{
    OPA_CONFIG_NAME            = 0,
    OPA_CONFIG_ALGO            = 8,
    OPA_CONFIG_VOLUME          = 9,
    OPA_CONFIG_PANNING         = 10,
    OPA_CONFIG_FLAGS           = 11,
}OPA_CONFIG_PARAMS;

typedef enum{
    OPA_OP_VOLUME              = 0,
    OPA_OP_COARSE              = 1,
    OPA_OP_FINE                = 2,
    OPA_OP_ENVATTACK           = 3,
    OPA_OP_ENVDECAY            = 4,
    OPA_OP_ENVSUSTAIN_LEVEL    = 5,
    OPA_OP_ENVINIT_LEVEL       = 6,
    OPA_OP_ENVRELEASE          = 7,
    OPA_OP_LFOSPEED            = 8,
    OPA_OP_LFOAMOUNT           = 9,
    OPA_OP_FEEDBACK            = 10,
    OPA_OP_FLAGS               = 11,
    OPA_OP_RESERVED2           = 12,
    OPA_OP_RESERVED3           = 13,
    OPA_OP_RESERVED4           = 14,
    OPA_OP_RESERVED5           = 15,
}OPA_OP_PARAMS;

/*****************************************************************************/
#define OPA_ALLPROGS_ID         0xFF

#define OPA_ALGOS_OP_NB         4
#define OPA_PROGS_NB            8

#define OPA_GLOBAL_PARAMS_NB    8
#define OPA_PROGS_PARAMS_NB     12
#define OPA_PROGS_NAME_LEN      8
#define OPA_PROGS_OP_PARAMS_NB  16
#define OPA_PROGS_PARAMS_TOTAL  (OPA_PROGS_PARAMS_NB + OPA_PROGS_OP_PARAMS_NB * OPA_ALGOS_OP_NB)

/*****************************************************************************/
typedef enum{
    OPA_GLOBAL_PROTECT	= 1,
    OPA_GLOBAL_DEFAULT  = OPA_GLOBAL_PROTECT,
}OPA_GLOBAL_FLAGSBITS;

typedef enum{
    OPA_PROGRAM_STEALING = 1,
    OPA_PROGRAM_DEFAULT  = OPA_PROGRAM_STEALING,
}OPA_PROGRAM_FLAGSBITS;

typedef enum{
    OPA_OP_ABSOLUTE     = 1,
    OPA_OP_SOFT_LOW     = 2,
    OPA_OP_HARD_LOW     = 4,
    OPA_OP_SOFT_HIGH    = 8,
    OPA_OP_HARD_HIGH    = 16,
    OPA_OP_MUTED        = 32,
    OPA_OP_DEFAULT      = 0,
}OPA_OP_FLAGSBITS;

/*****************************************************************************/
typedef struct{
    uint8_t volume;
    int8_t  coarse;
    int8_t  fine;
    uint8_t flags;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
}OpaGlobals;

typedef struct{
    uint8_t name[8];
    uint8_t algorithm;
    uint8_t volume;
    uint8_t panning;
    uint8_t flags;
}OpaProgramParams;

typedef struct{
    uint8_t volume;             //ok
    uint8_t coarse;             //ok
    int8_t  fine;               //ok
    uint8_t envAttack;          //ok
    uint8_t envDecay;           //ok
    uint8_t envSusLevel;        //ok
    uint8_t envIniLevel;        //ok
    uint8_t envRelease;         //ok
    uint8_t LFOSpeed;
    uint8_t LFOAmount;
    uint8_t feedback;
    uint8_t flags;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
}OpaOperatorParams;

/*****************************************************************************/
typedef struct{
    OpaProgramParams params;
    OpaOperatorParams opParams[OPA_ALGOS_OP_NB];
}OpaProgram;

/*****************************************************************************/
class Opa
{
public:
    Opa();

    void connect(int port);
    void disconnect();
    bool isConnected();

    void setEnable(bool enable);

    void update();

    void noteOn(int instrument, int note, int fraction);
    void noteOff(int instrument, int note, int fraction);
    void allNotesOff(int instrument);
    void allSoundsOff();

    void paramWrite(int program, int param, int value);
    void paramRead(int program, int param, int * value);
    void globalsParamWrite(int param, int value);
    void globalsParamRead(int param, int * value);

    void programWrite(int program, const OpaProgram * programData);
    void programRead(int program, OpaProgram * programData);
    void globalsWrite(const OpaGlobals * globalsData);
    void globalsRead(OpaGlobals * globalsData);

    void internalStore(int program, int slot);
    void internalLoad(int program, int slot);
    void internalWrite(int slot, const OpaProgram * programData);
    void internalRead(int slot, OpaProgram * programData);

    void pitchBend(int program, int coarse, int fine);

    bool isWaiting() {return NULL != globalsReturn || NULL != programReturn || NULL != paramReturn;}

public:
    static const OpaGlobals defaultGlobals;
    static const OpaProgramParams defaultProgramParams;
    static const OpaOperatorParams defaultOperatorParams;
    static OpaProgram defaultProgram;
    static void initDefaultProgram();

private:
    void parseParameter();
    void parseProgram();
    void parseGlobals();
    void fetchSerialData();

    int * paramReturn;
    int paramReturnIndex;

    volatile OpaProgram * programReturn;
    volatile OpaGlobals * globalsReturn;
    volatile int programReturnIndex;

    char rxBuffer[OPA_RXBUFFER_LEN];
    unsigned int rxLen;

    int port;
    int state;
    bool enabled;

};

#endif // OPA_H
