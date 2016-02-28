/**
    OPA Editor: OPA operator widget
    OPA operator widget

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

#include "programwidget.h"
#include "ui_programwidget.h"

#include <QMessageBox>
#include <QFileDialog>

#include "globals.h"
#include "programfile.h"
#include "mainwindow.h"

/*****************************************************************************/
ProgramWidget::ProgramWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgramWidget)
{
    programIndex = 0;
    ui->setupUi(this);
}

ProgramWidget::~ProgramWidget()
{
    delete ui;
}

/*****************************************************************************/
void ProgramWidget::setContent(const OpaProgramParams * params, bool send)
{
    opa.setEnable(false);

    QString name;
    programNameToQS(params->name, name);
    ui->nameLine->setText(name);
    ui->volumeDial->setValue(params->volume);
    ui->panningDial->setValue(params->panning);

    MainWindow * mw = MainWindow::getInstance();
    mw->setAlgorithm(params->algorithm);

    opa.setEnable(true);

    if (send) {
        for (int i = 0; i < 8; i++)
            opa.paramWrite(programIndex, OPA_CONFIG_ALGO, params->name[i]);
        opa.paramWrite(programIndex, OPA_CONFIG_ALGO, params->algorithm);
        opa.paramWrite(programIndex, OPA_CONFIG_VOLUME, params->volume);
        opa.paramWrite(programIndex, OPA_CONFIG_PANNING, params->panning);
        opa.paramWrite(programIndex, OPA_CONFIG_RESERVED, params->reserved);
        for(int i = 0; i < OPA_PROGS_NAME_LEN; i++)
            opa.paramWrite(programIndex, OPA_CONFIG_NAME + i, params->name[i]);
   }
}

void ProgramWidget::getContent(OpaProgramParams * params)
{
    programNameFromQS(ui->nameLine->text(), params->name);
    params->volume = ui->volumeDial->value();
    params->panning = ui->panningDial->value();

    MainWindow * mw = MainWindow::getInstance();
    params->algorithm = mw->getAlgorithm();
}

/*****************************************************************************/
void ProgramWidget::setProgramIndex(int index)
{
    programIndex = index;
}

/*****************************************************************************/
void ProgramWidget::on_volumeDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, OPA_CONFIG_VOLUME, value);
}

void ProgramWidget::on_panningDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, OPA_CONFIG_PANNING, value);
}

/*****************************************************************************/
void ProgramWidget::on_initButton_clicked()
{
// Init the program
    setContent(&Opa::defaultProgram, true);
    OpaOperatorParams params = Opa::defaultOperator;
    params.volume = 0xE0;
    editedOperators[0]->setContent(&params, true);
    for (int o = 1; o < OPA_ALGOS_OP_NB; o++)
        editedOperators[o]->setContent(&Opa::defaultOperator, true);

// Refresh the window
    MainWindow * mw = MainWindow::getInstance();
    mw->refresh();
}


/*****************************************************************************/
void ProgramWidget::on_openButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open OPA preset", QString(), "Presets Files (*.xml)");
    if (fileName.isEmpty()) return;

// Load the program
    OpaProgram prog;
    ProgramFile progFile(fileName);
    progFile.load(&prog);
    setContent(&prog.params, true);
    for (int o = 0; o < OPA_ALGOS_OP_NB; o++)
        editedOperators[o]->setContent(&prog.opParams[o], true);

// Refresh the window
    MainWindow * mw = MainWindow::getInstance();
    mw->refresh();
}

void ProgramWidget::on_saveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save OPA preset", QString(), "Presets Files (*.xml)");
    if (fileName.isEmpty()) return;

// Read the interface
    OpaProgram prog;
    getContent(&prog.params);
    for (int o = 0; o < OPA_ALGOS_OP_NB; o++)
        editedOperators[o]->getContent(&prog.opParams[o]);

// Save the program
    ProgramFile progFile(fileName);
    progFile.save(&prog);
}

/*****************************************************************************/
void ProgramWidget::on_loadButton_clicked()
{
    int slot = ui->slotSpin->value() - 1;
    if (slot < 0) return;
    opa.programLoad(programIndex, slot);
}

void ProgramWidget::on_storeButton_clicked()
{
    QMessageBox mb(QMessageBox::Warning, "OPA Editor", "This will discard any previously stored program, continue?", QMessageBox::Yes | QMessageBox::No, this);
    if (mb.exec() == QMessageBox::No) return;
    int slot = ui->slotSpin->value() - 1;
    if (slot < 0) return;
    opa.programStore(programIndex, slot);
}

/*****************************************************************************/
void ProgramWidget::on_nameLine_editingFinished()
{
    uint8_t name[OPA_PROGS_NAME_LEN];
    programNameFromQS(ui->nameLine->text(), name);
    for(int i = 0; i < OPA_PROGS_NAME_LEN; i++)
        opa.paramWrite(programIndex, OPA_CONFIG_NAME + i, name[i]);
}
