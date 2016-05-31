/**
    OPA Editor: Mainwindow
    Editor Mainwindow class

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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

#include "globals.h"
#include "rs232/rs232.h"

/*****************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    midiIn(NULL)
{
    mainWindow = this;
    ui->setupUi(this);

    memset(&programBuffer, 0, sizeof(OpaProgram));
    memset(&globalsBuffer, 0, sizeof(OpaGlobals));

    needGlobalsRefresh = false;
    needProgramRefresh = false;
    needAllProgramRefresh = false;
    waitforGlobals = false;
    waitforProgram = false;
    waitforParam = false;

    setProgram(0);
    setAlgorithm(0);
    setPage(0);

    for (int i = 0; i < 16; i++)
        midiChannelsAct[i] = false;
    for (int i = 0; i < 32; i++)
        midiSamplesAct[i] = false;
    midiBendRange = 4;

    comTimer = new QTimer(this);
    connect(comTimer, SIGNAL(timeout()), this, SLOT(comTimer_timeout()));
    comTimer->start(5);

    UITimer = new QTimer(this);
    connect(UITimer, SIGNAL(timeout()), this, SLOT(UITimer_timeout()));
    UITimer->start(20);

    statusLabel = new QLabel(this);
    ui->statusBar->addWidget(statusLabel);

    devicesEnumerate();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete comTimer;
    delete UITimer;
    delete statusLabel;
}

/*****************************************************************************/
#define MENU_ENTRY_CONFIG   0x0000
#define MENU_ENTRY_ARDUINO  0x1000
#define MENU_ENTRY_MIDI     0x2000
#define getMenuEntry(a)     ((a)->data().toInt() & 0xF000)
#define getMenuIndex(a)     ((a)->data().toInt() & 0x0FFF)
void MainWindow::devicesEnumerate()
{
    QMenu * menu = ui->connectionMenu;
    menu->clear();

// Enumerate COM devices
    opa.disconnect();
    int portNb = comEnumerate();
    for (int i = 0; i < portNb; i++) {
        QString action("Arduino on: ");
        action.append(comGetPortName(i));
        QAction * ma = menu->addAction(action);
        ma->setData(MENU_ENTRY_ARDUINO + i);
        ma->setCheckable(true);
    }
    menu->addSeparator();

// Enumerate MIDI devices
    if (midiIn) delete midiIn;
    midiIn = NULL;
    portNb = MidiIn::getNoDevices();
    for (int i = 0; i < portNb; i++) {
        QString action("MIDI on: ");
        action.append(MidiIn::getDeviceInName(i));
        QAction * ma = menu->addAction(action);
        ma->setData(MENU_ENTRY_MIDI + i);
        ma->setCheckable(true);

    }

// Set the refresh action
    menu->addSeparator();
    QString action("Refresh");
    QAction * ma = menu->addAction(action);
    ma->setData(MENU_ENTRY_CONFIG);

// Show status
    statusLabel->setText("Arduino is not connected");
}

/*****************************************************************************/
void MainWindow::refresh()
{
    setProgram(currentProgram);
    setAlgorithm(currentAlgorithm);
}

/*****************************************************************************/
void MainWindow::on_fileMenu_triggered(QAction * action)
{
    if (action == ui->quitAction) {
        comCloseAll();
        close();
    }
}

void MainWindow::on_connectionMenu_triggered(QAction * action)
{
    QMenu * menu = ui->connectionMenu;
    QList<QAction *> actionsList = menu->actions();
    int actionsNb = actionsList.count();

// Connect an Arduino board
    int entry = getMenuEntry(action);
    switch(entry) {

        case MENU_ENTRY_CONFIG:
            devicesEnumerate();
        break;

        case MENU_ENTRY_ARDUINO:
        // Setup communication
            opa.connect(getMenuIndex(action));
            waitforProgram = false;
            waitforParam = false;
            waitforGlobals = false;

            needGlobalsRefresh = false;
            needProgramRefresh = false;
            allProgramCount = -1;
            needAllProgramRefresh = true;

        // Mark the menu
            for (int j = 0; j < actionsNb; j++)
                if (getMenuEntry(actionsList[j]) == MENU_ENTRY_ARDUINO)
                    actionsList[j]->setChecked(false);
            if (opa.isConnected()) {
                statusLabel->setText("Arduino is connected");
                action->setChecked(true);
            }else statusLabel->setText("Arduino is not connected");

        break;

        case MENU_ENTRY_MIDI:
        // Setup communication
            if (midiIn) delete midiIn;
            midiIn = new MidiIn(getMenuIndex(action), MainWindow::midiInCallback);

        // Mark the menu
            for (int j = 0; j < actionsNb; j++)
                if (getMenuEntry(actionsList[j]) == MENU_ENTRY_MIDI)
                    actionsList[j]->setChecked(false);
            action->setChecked(true);
        break;

        default: break;
    }

}

void MainWindow::on_deviceMenu_triggered(QAction * action)
{
    if (action == ui->allNotesOffAction) {
        opa.allNotesOff(currentProgram);
    }else if (action == ui->allSoundsOffAction) {
        opa.allSoundsOff();
    }else if (action == ui->memoryProtectionAction) {
        writeFlags();
    }
}


/*****************************************************************************/
void MainWindow::on_helpMenu_triggered(QAction * action)
{
    if (action == ui->aboutAction) {
        QMessageBox msgBox;
        msgBox.setText("OPA - Soundchip editor");
        msgBox.setInformativeText(
            "Software & hardware developpment: Frederic Meslin\n"
            "Project design: Thomas Hopper\n\n"
            "(c) Frederic Meslin / Thomas Hopper 2015-2016\n"
            "http://fredslab.net\n\n"
            "Version 0.85 18/03/2016\n\n"
            "Software distributed under open-source MIT license, please refer to licence.txt for more details\n"
        );
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}

/*****************************************************************************/
void MainWindow::comTimer_timeout()
{
    opa.update();
}

void MainWindow::UITimer_timeout()
{
    if (waitforGlobals && !opa.isWaitingGlobals()) {
    // Update UI globals
        setFlags(globalsBuffer.flags);
        ui->pageMixing->setGlobalsContent(&globalsBuffer, false);
        waitforGlobals = false;

    }else if (waitforProgram && !opa.isWaitingProgram()) {
    // Update UI program
        if (allProgramCount == currentProgram) {
            editedProgram->setContent(&programBuffer.params, false);
            for (int o = 0; o < OPA_ALGOS_OP_NB; o++)
                editedOperators[o]->setContent(&programBuffer.opParams[o], false);
        }
        ui->pageMixing->setContent(allProgramCount, &programBuffer.params, false);
        waitforProgram = false;

    }else if (waitforParam && !opa.isWaitingParam()) {
        waitforParam = false;
    }else if (needGlobalsRefresh) {
        globalRead();
        needGlobalsRefresh = false;
    }else if (needProgramRefresh) {
        allProgramCount = currentProgram;
        programRead(currentProgram);
        needProgramRefresh = false;
    }else if (needAllProgramRefresh) {
        programRead(++allProgramCount);
        if (allProgramCount == OPA_PROGS_NB - 1)
            needAllProgramRefresh = false;
            needGlobalsRefresh = true;
    }


    editedProgram->updateUI();
    for (int o = 0; o < 4; o++)
        editedOperators[o]->updateUI();

    ui->pagePrograms->refreshProgramLeds();
    ui->pageMixing->refreshMixingLeds();
    ui->pageMixing->refreshSampleLeds();
    for (int i = 0; i < 16; i++)
        midiChannelsAct[i] = 0;
    for (int i = 0; i < 32; i++)
        midiSamplesAct[i] = 0;

}


/*****************************************************************************/
#define MIDI_DRUMS_CHANNEL      9
#define MIDI_PROGMAP_CHANNEL    15

void MainWindow::midiInCallback(uint8_t msg[])
{
    uint8_t s = msg[0] & 0xF0;
    uint8_t c = msg[0] & 0x0F;
    midiChannelsAct[c] = true;

    switch (s) {
        case 0x80:
            if (c == MIDI_PROGMAP_CHANNEL && mainWindow->ui->ProgMappingAction->isChecked()) {
                progMap(msg);
                break;
            }

            if (c == MIDI_DRUMS_CHANNEL)
                midiSamplesAct[msg[1]%32] = true;

            opa.noteOff(c, msg[1], 0);
            break;

        case 0x90:
            if (c == MIDI_PROGMAP_CHANNEL && mainWindow->ui->ProgMappingAction->isChecked()) {
                progMap(msg);
                break;
            }

            if (c == MIDI_DRUMS_CHANNEL)
                midiSamplesAct[msg[1]%32] = true;

            if (msg[2]) opa.noteOn(c, msg[1], 0);
            else opa.noteOff(c, msg[1], 0);
            break;

        case 0xB0:
            if (msg[1] == 0x78) opa.allSoundsOff();
            else if (msg[1] == 0x7B) opa.allNotesOff(c);
            else if (mainWindow->ui->CCMappingAction->isChecked()) {
                if (msg[1] >= 10 && msg[1] < 54) {
                    int i = msg[1] - 10;
                    int o = i % 4;
                    int p = i / 4;
                    editedOperators[o]->updateSingle(p, msg[2] << 1);
                }
            }
            break;

        case 0xE0:
            int16_t bend = (msg[2] << 7) + msg[1] - 0x2000;
            bend >>= midiBendRange;
            int8_t coarse = bend >> 8;
            int8_t fine = (bend & 0xFF) >> 1;
            opa.pitchBend(c, coarse, fine);
            break;

    }
}

/*****************************************************************************/
void MainWindow::progMap(uint8_t msg[])
{
    int c = msg[1] % OPA_PROGS_NB;
    int n = 36 + msg[1] / OPA_PROGS_NB;
    if (msg[2]) opa.noteOn(c, n, 0);
    else opa.noteOff(c, n, 0);
}

/*****************************************************************************/
void MainWindow::programRead(int program)
{
    if (opa.isWaitingProgram()) return;
    memset(&programBuffer, 0, sizeof(OpaProgram));
    opa.programRead(program, &programBuffer);
    waitforProgram = true;
}


void MainWindow::programWrite(int program)
{
}

/*****************************************************************************/
void MainWindow::globalRead()
{
    if (opa.isWaitingProgram()) return;
    memset(&globalsBuffer, 0, sizeof(OpaGlobals));
    opa.globalsRead(&globalsBuffer);
    waitforGlobals = true;
}

void MainWindow::globalWrite()
{

}

/*****************************************************************************/
void MainWindow::setProgram(int program)
{
    programRead(program);
    editedProgram->setProgramIndex(program);
    for (int o = 0; o < OPA_ALGOS_OP_NB; o++)
        editedOperators[o]->setProgram(program);

    ui->pagePrograms->setProgram(program);
    currentProgram = program;
}

void MainWindow::setAlgorithm(int algorithm)
{
    ui->pagePrograms->setAlgorithm(algorithm);
    currentAlgorithm = algorithm;
}

void MainWindow::setPage(int page)
{
    allProgramCount = -1;
    needAllProgramRefresh = true;

    ui->pagePrograms->setVisible(0 == page);
    ui->pageMixing->setVisible(1 == page);

    ui->programsPagePush->setChecked(0 == page);
    ui->mixingPagePush->setChecked(1 == page);
}

/*****************************************************************************/
void MainWindow::writeFlags()
{
//TODO: chek that
    int flags = 0;
    flags |= ui->memoryProtectionAction->isChecked() ? OPA_GLOBAL_PROTECT : 0;
    opa.globalsParamWrite(OPA_GLOBAL_FLAGS, flags);
}

void MainWindow::setFlags(int flags)
{
    ui->memoryProtectionAction->setChecked(flags & OPA_GLOBAL_PROTECT);
}

/*****************************************************************************/
bool MainWindow::getMemoryProtection()
{
    return ui->memoryProtectionAction->isChecked();
}

/*****************************************************************************/
void MainWindow::on_programsPagePush_clicked()
{
    setPage(0);
}

void MainWindow::on_mixingPagePush_clicked()
{
    setPage(1);
}
