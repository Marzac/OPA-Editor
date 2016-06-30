#include "programspage.h"
#include "ui_programspage.h"

#include "globals.h"
#include "rs232/rs232.h"
#include "midi/midiin.h"

#include "mainwindow.h"

/*****************************************************************************/
ProgramsPage::ProgramsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgramsPage)
{
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

}

ProgramsPage::~ProgramsPage()
{
    delete ui;
}


/*****************************************************************************/
void ProgramsPage::setProgram(int program)
{
    ui->i1Push->setChecked(program == 0);
    ui->i2Push->setChecked(program == 1);
    ui->i3Push->setChecked(program == 2);
    ui->i4Push->setChecked(program == 3);
    ui->i5Push->setChecked(program == 4);
    ui->i6Push->setChecked(program == 5);
    ui->i7Push->setChecked(program == 6);
    ui->i8Push->setChecked(program == 7);
}

void ProgramsPage::setAlgorithm(int a)
{
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
void ProgramsPage::refreshProgramLeds()
{
    for (int c = 0; c < 8; c++){
        if (midiChannelsAct[c]) {
            programLeds[c]->luminosity = 0xF000;
            programLeds[c]->repaint();
        }else if (programLeds[c]->luminosity >= 0x1000) {
            programLeds[c]->luminosity -= 0x1000;
            programLeds[c]->repaint();
        }
    }
}

/*****************************************************************************/
void ProgramsPage::on_i1Push_clicked()
{
    mainWindow->setProgram(0);
}

void ProgramsPage::on_i2Push_clicked()
{
    mainWindow->setProgram(1);
}

void ProgramsPage::on_i3Push_clicked()
{
    mainWindow->setProgram(2);
}

void ProgramsPage::on_i4Push_clicked()
{
    mainWindow->setProgram(3);
}

void ProgramsPage::on_i5Push_clicked()
{
    mainWindow->setProgram(4);
}

void ProgramsPage::on_i6Push_clicked()
{
    mainWindow->setProgram(5);
}

void ProgramsPage::on_i7Push_clicked()
{
    mainWindow->setProgram(6);
}

void ProgramsPage::on_i8Push_clicked()
{
    mainWindow->setProgram(7);
}

/*****************************************************************************/
void ProgramsPage::on_algo1Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 1);
    mainWindow->setAlgorithm(1);
}

void ProgramsPage::on_algo2Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 2);
    mainWindow->setAlgorithm(2);
}

void ProgramsPage::on_algo3Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 3);
    mainWindow->setAlgorithm(3);
}

void ProgramsPage::on_algo4Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 4);
    mainWindow->setAlgorithm(4);
}

void ProgramsPage::on_algo5Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 5);
    mainWindow->setAlgorithm(5);
}

void ProgramsPage::on_algo6Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 6);
    mainWindow->setAlgorithm(6);
}

void ProgramsPage::on_algo7Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 7);
    mainWindow->setAlgorithm(7);
}

void ProgramsPage::on_algo8Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 8);
    mainWindow->setAlgorithm(8);
}

void ProgramsPage::on_algo9Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 9);
    mainWindow->setAlgorithm(9);
}

void ProgramsPage::on_algo10Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 10);
    mainWindow->setAlgorithm(10);
}

void ProgramsPage::on_algo11Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 11);
    mainWindow->setAlgorithm(11);
}

void ProgramsPage::on_algo12Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 12);
    mainWindow->setAlgorithm(12);
}

void ProgramsPage::on_algo13Push_clicked()
{
    opa.programParamWrite(currentProgram, OPA_CONFIG_ALGO, 13);
    mainWindow->setAlgorithm(13);
}

