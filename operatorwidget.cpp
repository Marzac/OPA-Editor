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

#include "operatorwidget.h"
#include "ui_operatorwidget.h"

#include "globals.h"

/*****************************************************************************/
OperatorWidget::OperatorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OperatorWidget)
{
    operatorIndex = 0;
    programIndex = 0;
    ui->setupUi(this);
}

OperatorWidget::~OperatorWidget()
{
    delete ui;
}

/*****************************************************************************/
void OperatorWidget::setContent(const OpaOperatorParams * params, bool send)
{
    QString text;
    opa.setEnable(false);

    setFlags(params->flags);
    ui->volumeSlide->setValue(params->volume);
    on_volumeSlide_valueChanged(params->volume);

    int coarse = params->flags & OPA_OP_ABSOLUTE ?
        ((int8_t) params->coarse) + 128 : params->coarse;

    ui->coarseDial->setValue(coarse);
    on_coarseDial_valueChanged(coarse);

    ui->fineDial->setValue(params->fine);
    on_fineDial_valueChanged(params->fine);

    ui->LFOSpeedDial->setValue(params->LFOSpeed);
    ui->LFOAmountDial->setValue(params->LFOAmount);
    ui->feedbackDial->setValue(params->feedback);

    ui->attackDial->setValue(params->envAttack);
    ui->decayDial->setValue(params->envDecay);
    ui->sustainDial->setValue(params->envSusLevel);
    ui->initDial ->setValue(params->envIniLevel);
    ui->releaseDial->setValue(params->envRelease);

    updateEnvelope();

    opa.setEnable(true);

    if (send) {
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_VOLUME), params->volume);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_COARSE), params->coarse);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_FINE), params->fine);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_LFOSPEED), params->LFOSpeed);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_LFOAMOUNT), params->LFOAmount);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_FEEDBACK), params->feedback);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVATTACK), params->envAttack);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVDECAY), params->envDecay);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVSUSTAIN_LEVEL), params->envSusLevel);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVINIT_LEVEL), params->envIniLevel);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVRELEASE), params->envRelease);

        writeFlags();
   }
}

void OperatorWidget::getContent(OpaOperatorParams * params)
{
    params->flags = getFlags();
    params->volume = ui->volumeSlide->value();

    params->coarse = ui->coarseDial->value();
    params->fine = ui->fineDial->value();
    params->LFOSpeed = ui->LFOSpeedDial->value();
    params->LFOAmount = ui->LFOAmountDial->value();
    params->feedback = ui->feedbackDial->value();

    params->envAttack = ui->attackDial->value();
    params->envDecay = ui->decayDial->value();
    params->envSusLevel = ui->sustainDial->value();
    params->envIniLevel = ui->initDial->value();
    params->envRelease = ui->releaseDial->value();

}

/*****************************************************************************/
void OperatorWidget::setOperator(int index, bool feedback)
{
    operatorIndex = index;
    ui->feedbackDial->setVisible(feedback);
    ui->feedbackLabel->setVisible(feedback);
}

void OperatorWidget::setProgram(int program)
{
    programIndex = program;
}

/*****************************************************************************/
void OperatorWidget::update()
{
}

/*****************************************************************************/
const float opaRatios[] = {
    0.2000f, 0.2500f, 0.3333f, 0.5000f,
    1.0000f, 2.0000f, 3.0000f, 4.0000f,
    5.0000f, 6.0000f, 7.0000f, 8.0000f,
    9.0000f, 10.000f, 11.000f, 12.000f,
    13.000f, 14.000f, 15.000f, 16.000f,
    17.000f, 18.000f, 19.000f, 20.000f,
    21.000f, 22.000f, 23.000f, 24.000f,
    25.000f, 26.000f, 27.000f, 28.000f
};

void OperatorWidget::on_coarseDial_valueChanged(int value)
{
    QString text;
    if (ui->absoluteButton->isChecked()) {
        QString semi;
        semi.setNum(value - 128, 10);
        text = semi + " semi";
        ui->coarseLine->setText(text);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_COARSE), value - 128);
    }else{
        QString mul;
        float v = opaRatios[value >> 3];
        mul.setNum(v, 'f', 2);
        text = "x" + mul;
        ui->coarseLine->setText(text);
        opa.paramWrite(programIndex, getParamIndex(OPA_OP_COARSE), value);
    }
}

void OperatorWidget::on_fineDial_valueChanged(int value)
{
    QString fine;
    fine.setNum(value, 10);
    ui->fineLine->setText(fine);
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_FINE), value);
}

void OperatorWidget::on_absoluteButton_clicked(bool checked)
{
    on_coarseDial_valueChanged(ui->coarseDial->value());
    ui->coarseLine->setReadOnly(!checked);
    writeFlags();
}

/*****************************************************************************/
void OperatorWidget::on_attackDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVATTACK), value);
    updateEnvelope();
}

void OperatorWidget::on_decayDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVDECAY), value);
    updateEnvelope();
}

void OperatorWidget::on_sustainDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVSUSTAIN_LEVEL), value);
    updateEnvelope();
}

void OperatorWidget::on_initDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVINIT_LEVEL), value);
    updateEnvelope();
}

void OperatorWidget::on_releaseDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_ENVRELEASE), value);
    updateEnvelope();
}

/*****************************************************************************/
void OperatorWidget::on_LFOSpeedDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_LFOSPEED), value);
}

void OperatorWidget::on_LFOAmountDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_LFOAMOUNT), value);
}

/*****************************************************************************/
void OperatorWidget::on_feedbackDial_valueChanged(int value)
{
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_FEEDBACK), value);
}

/*****************************************************************************/
void OperatorWidget::on_muteButton_clicked(bool checked)
{
    writeFlags();
}

void OperatorWidget::on_volumeSlide_valueChanged(int value)
{
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_VOLUME), value);
    QString volume;
    volume.setNum(value, 10);
    ui->volumeLine->setText(volume);
}

/*****************************************************************************/
void OperatorWidget::on_trackSoftHighButton_clicked()
{
    writeFlags();
}

void OperatorWidget::on_trackHardHighButton_clicked()
{
    writeFlags();
}

void OperatorWidget::on_trackSoftLowButton_clicked()
{
    writeFlags();
}

void OperatorWidget::on_trackHardLowButton_clicked()
{
    writeFlags();
}

/*****************************************************************************/
void OperatorWidget::writeFlags()
{
    int flags = getFlags();
    opa.paramWrite(programIndex, getParamIndex(OPA_OP_FLAGS), flags);
}

/*****************************************************************************/
void OperatorWidget::setFlags(int flags)
{
    ui->absoluteButton->setChecked(flags & OPA_OP_ABSOLUTE);
    ui->trackSoftLowButton->setChecked(flags & OPA_OP_SOFT_LOW);
    ui->trackHardLowButton->setChecked(flags & OPA_OP_HARD_LOW);
    ui->trackSoftHighButton->setChecked(flags & OPA_OP_SOFT_HIGH);
    ui->trackHardHighButton->setChecked(flags & OPA_OP_HARD_HIGH);
    ui->muteButton->setChecked(flags & OPA_OP_MUTED);

    ui->coarseLine->setReadOnly(!(flags & OPA_OP_ABSOLUTE));
}

int OperatorWidget::getFlags()
{
    int flags = 0;
    flags |= ui->absoluteButton->isChecked() ? OPA_OP_ABSOLUTE : 0;
    flags |= ui->trackSoftLowButton->isChecked() ? OPA_OP_SOFT_LOW : 0;
    flags |= ui->trackHardLowButton->isChecked() ? OPA_OP_HARD_LOW : 0;
    flags |= ui->trackSoftHighButton->isChecked() ? OPA_OP_SOFT_HIGH : 0;
    flags |= ui->trackHardHighButton->isChecked() ? OPA_OP_HARD_HIGH : 0;
    flags |= ui->muteButton->isChecked() ? OPA_OP_MUTED : 0;
    return flags;
}

/*****************************************************************************/
void OperatorWidget::updateEnvelope()
{
    ui->envelopeWidget->a = ui->attackDial->value();
    ui->envelopeWidget->d = ui->decayDial->value();
    ui->envelopeWidget->s = ui->sustainDial->value();
    ui->envelopeWidget->i = ui->initDial->value();
    ui->envelopeWidget->r = ui->releaseDial->value();
    ui->envelopeWidget->repaint();
}

/*****************************************************************************/
int OperatorWidget::getParamIndex(int p)
{
    return operatorIndex * OPA_PROGS_OP_PARAMS_NB + OPA_PROGS_PARAMS_NB + p;
}


/*****************************************************************************/
void OperatorWidget::on_coarseLine_editingFinished()
{
    bool ok = false;
    QString line = ui->coarseLine->text();
    int v = line.toInt(&ok);
    if (!ok) {line.chop(4); v = line.toInt(&ok);}
    if (!ok) {
        on_coarseDial_valueChanged(ui->coarseDial->value());
    }else{
        v += 128;
        if (v > 255) {v = 255; ui->coarseLine->setText("127 semi");}
        else if (v < 0) {v = 0; ui->coarseLine->setText("-128 semi");}
        ui->coarseDial->setValue(v);

    }
}

void OperatorWidget::on_fineLine_editingFinished()
{
    bool ok = false;
    int v = ui->fineLine->text().toInt(&ok);
    if (!ok) {
        on_fineDial_valueChanged(ui->fineDial->value());
    }else{
        if (v > 127) {v = 127; ui->fineLine->setText("127");}
        else if (v < -128) {v = -128; ui->fineLine->setText("-128");}
        ui->fineDial->setValue(v);
    }
}

/*****************************************************************************/
void OperatorWidget::on_volumeLine_editingFinished()
{
    bool ok = false;
    int v = ui->volumeLine->text().toInt(&ok);
    if (!ok) {
        on_volumeSlide_valueChanged(ui->volumeSlide->value());
    }else{
        if (v > 255) {v = 255; ui->volumeLine->setText("255");}
        else if (v < 0) {v = 0; ui->volumeLine->setText("0");}
        ui->volumeSlide->setValue(v);
    }
}

