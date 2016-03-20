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
MainWindow * MainWindow::instance = NULL;
int MainWindow::midiBendRange = 4;
bool MainWindow::midiChannelsAct[16];

/*****************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    midiIn(NULL)
{
    instance = this;

    ui->setupUi(this);
    editedOperators[0] = (OperatorWidget *) ui->op1Widget;
    editedOperators[1] = (OperatorWidget *) ui->op2Widget;
    editedOperators[2] = (OperatorWidget *) ui->op3Widget;
    editedOperators[3] = (OperatorWidget *) ui->op4Widget;
    editedProgram = (ProgramWidget *) ui->programWidget;

    programLeds[0] = (Led *) ui->i1Led;
    programLeds[1] = (Led *) ui->i2Led;
    programLeds[2] = (Led *) ui->i3Led;
    programLeds[3] = (Led *) ui->i4Led;
    programLeds[4] = (Led *) ui->i5Led;
    programLeds[5] = (Led *) ui->i6Led;
    programLeds[6] = (Led *) ui->i7Led;
    programLeds[7] = (Led *) ui->i8Led;

    for (int o = 0; o < 4; o++)
        editedOperators[o]->setOperator(o, o == 3);

    for (int i = 0; i < 16; i++)
        midiChannelsAct[i] = false;

    memset(&programBuffer, 0, sizeof(OpaProgram));
    memset(&globalsBuffer, 0, sizeof(OpaGlobals));

    needGlobalsRefresh = false;
    needProgramRefresh = false;
    waitforGlobals = false;
    waitforProgram = false;
    waitforParam = false;
    connectDelay = 0;

    setProgram(0);
    setAlgorithm(0);

    on_masterTuneDial_valueChanged(0);
    on_masterVolumeDial_valueChanged(0);

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
    delete statusLabel;
}

/*****************************************************************************/
MainWindow * MainWindow::getInstance()
{
    return instance;
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
    setProgram(programIndex);
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
            needGlobalsRefresh = true;
            needProgramRefresh = true;
            connectDelay = 3000;

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
        opa.allNotesOff(programIndex);
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
    if (connectDelay > 0) {
        connectDelay -= 20;
    }else if (waitforGlobals && !opa.isWaiting()) {
        setFlags(globalsBuffer.flags);
        ui->masterVolumeDial->setValue(globalsBuffer.volume);
        ui->masterTuneDial->setValue(globalsBuffer.coarse);
        waitforGlobals = false;
    }else if (waitforProgram && !opa.isWaiting()) {
        editedProgram->setContent(&programBuffer.params, false);
        for (int o = 0; o < 4; o++)
            editedOperators[o]->setContent(&programBuffer.opParams[o], false);
        waitforProgram = false;
    }else if (waitforParam && !opa.isWaiting()) {
        waitforParam = false;
    }else if (needGlobalsRefresh) {
        globalRead();
        needGlobalsRefresh = false;
    }else if (needProgramRefresh) {
        programRead(programIndex);
        needProgramRefresh = false;
    }

    editedProgram->updateUI();
    for (int o = 0; o < 4; o++)
        editedOperators[o]->updateUI();

    refreshProgramLeds();
}


/*****************************************************************************/
void MainWindow::refreshProgramLeds()
{
    for (int c = 0; c < 8; c++){
        if (midiChannelsAct[c]) {
            programLeds[c]->luminosity = 0xF000;
            programLeds[c]->repaint();
        }else if (programLeds[c]->luminosity >= 0x1000) {
            programLeds[c]->luminosity -= 0x1000;
            programLeds[c]->repaint();
        }
        midiChannelsAct[c] = false;
    }
}

/*****************************************************************************/
#define MIDI_DRUMS_CHANNEL  9

void MainWindow::midiInCallback(uint8_t msg[])
{
    MainWindow * mw = MainWindow::getInstance();

    uint8_t s = msg[0] & 0xF0;
    uint8_t c = msg[0] & 0x0F;
    midiChannelsAct[c] = true;

    switch (s) {
        case 0x80:
            if (c == MIDI_DRUMS_CHANNEL) {
                if (mw->ui->DrumMappingAction->isChecked()) drumMap(msg);
            }else opa.noteOff(c, msg[1], 0);
            break;
        case 0x90:
            if (c == MIDI_DRUMS_CHANNEL) {
                if (mw->ui->DrumMappingAction->isChecked()) drumMap(msg);
            }else{
                if (msg[2]) opa.noteOn(c, msg[1], 0);
                else opa.noteOff(c, msg[1], 0);
            }
            break;

        case 0xB0:
            if (msg[1] == 0x78) opa.allSoundsOff();
            else if (msg[1] == 0x7B) opa.allNotesOff(c);
            else if (mw->ui->CCMappingAction->isChecked()) {
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
void MainWindow::drumMap(uint8_t msg[])
{
    int c = msg[1] % OPA_PROGS_NB;
    int n = 36 + msg[1] / OPA_PROGS_NB;
    if (msg[2]) opa.noteOn(c, n, 0);
    else opa.noteOff(c, n, 0);
}

/*****************************************************************************/
void MainWindow::programRead(int program)
{
    if (opa.isWaiting()) return;
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
    if (opa.isWaiting()) return;
    memset(&globalsBuffer, 0, sizeof(OpaGlobals));
    opa.globalsRead(&globalsBuffer);
    waitforGlobals = true;
}

void MainWindow::globalWrite()
{

}

/*****************************************************************************/
void MainWindow::on_i1Push_clicked()
{
    setProgram(0);
}

void MainWindow::on_i2Push_clicked()
{
    setProgram(1);
}

void MainWindow::on_i3Push_clicked()
{
    setProgram(2);
}

void MainWindow::on_i4Push_clicked()
{
    setProgram(3);
}

void MainWindow::on_i5Push_clicked()
{
    setProgram(4);
}

void MainWindow::on_i6Push_clicked()
{
    setProgram(5);
}

void MainWindow::on_i7Push_clicked()
{
    setProgram(6);
}

void MainWindow::on_i8Push_clicked()
{
    setProgram(7);
}

/*****************************************************************************/
void MainWindow::setProgram(int program)
{
    programRead(program);
    editedProgram->setProgramIndex(program);
    for (int o = 0; o < 4; o++)
        editedOperators[o]->setProgram(program);

    ui->i1Push->setChecked(program == 0);
    ui->i2Push->setChecked(program == 1);
    ui->i3Push->setChecked(program == 2);
    ui->i4Push->setChecked(program == 3);
    ui->i5Push->setChecked(program == 4);
    ui->i6Push->setChecked(program == 5);
    ui->i7Push->setChecked(program == 6);
    ui->i8Push->setChecked(program == 7);

    programIndex = program;
}


/*****************************************************************************/
void MainWindow::on_algo1Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 1);
    setAlgorithm(1);
}

void MainWindow::on_algo2Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 2);
    setAlgorithm(2);
}

void MainWindow::on_algo3Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 3);
    setAlgorithm(3);
}

void MainWindow::on_algo4Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 4);
    setAlgorithm(4);
}

void MainWindow::on_algo5Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 5);
    setAlgorithm(5);
}

void MainWindow::on_algo6Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 6);
    setAlgorithm(6);
}

void MainWindow::on_algo7Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 7);
    setAlgorithm(7);
}

void MainWindow::on_algo8Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 8);
    setAlgorithm(8);
}

void MainWindow::on_algo9Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 9);
    setAlgorithm(9);
}

void MainWindow::on_algo10Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 10);
    setAlgorithm(10);
}

void MainWindow::on_algo11Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 11);
    setAlgorithm(11);
}

void MainWindow::on_algo12Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 12);
    setAlgorithm(12);
}

void MainWindow::on_algo13Push_clicked()
{
    opa.paramWrite(programIndex, OPA_CONFIG_ALGO, 13);
    setAlgorithm(13);
}

/*****************************************************************************/
void MainWindow::on_masterTuneDial_valueChanged(int value)
{
    QString semi;
    semi.setNum(value, 10);
    ui->masterTuneEditLabel->setText(semi + " semi");
    opa.globalsParamWrite(OPA_GLOBAL_COARSE, value);
}


void MainWindow::on_masterVolumeDial_valueChanged(int value)
{
    QString volume;
    volume.setNum(value, 10);
    ui->masterVolumeEditLabel->setText(volume);
    opa.globalsParamWrite(OPA_GLOBAL_VOLUME, value);
}

/*****************************************************************************/
void MainWindow::setAlgorithm(int a)
{
    algorithm = a;
    ui->algo1Push->setChecked(a == 1);
    ui->algo2Push->setChecked(a == 2);
    ui->algo3Push->setChecked(a == 3);
    ui->algo4Push->setChecked(a == 4);
    ui->algo5Push->setChecked(a == 5);
    ui->algo6Push->setChecked(a == 6);
    ui->algo7Push->setChecked(a == 7);
    ui->algo8Push->setChecked(a == 8);
    ui->algo9Push->setChecked(a == 9);
    ui->algo10Push->setChecked(a == 10);
    ui->algo11Push->setChecked(a == 11);
    ui->algo12Push->setChecked(a == 12);
    ui->algo13Push->setChecked(a == 13);
}

/*****************************************************************************/
void MainWindow::writeFlags()
{
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
