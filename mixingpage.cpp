#include "mixingpage.h"
#include "ui_mixingpage.h"

#include "globals.h"
#include "rs232/rs232.h"
#include "midi/midiin.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"  //TODO: remove that

MixingPage::MixingPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MixingPage)
{
    ui->setupUi(this);
    mixingLeds[0] = (Led *) ui->prog1Led;
    mixingLeds[1] = (Led *) ui->prog2Led;
    mixingLeds[2] = (Led *) ui->prog3Led;
    mixingLeds[3] = (Led *) ui->prog4Led;
    mixingLeds[4] = (Led *) ui->prog5Led;
    mixingLeds[5] = (Led *) ui->prog6Led;
    mixingLeds[6] = (Led *) ui->prog7Led;
    mixingLeds[7] = (Led *) ui->prog8Led;
    mixingLeds[8] = (Led *) NULL;
    mixingLeds[9] = (Led *) ui->mixerKitLed;

    kitLeds[0] = (Led *) ui->sample1Led;
    kitLeds[1] = (Led *) ui->sample2Led;
    kitLeds[2] = (Led *) ui->sample3Led;
    kitLeds[3] = (Led *) ui->sample4Led;
    kitLeds[4] = (Led *) ui->sample5Led;
    kitLeds[5] = (Led *) ui->sample6Led;
    kitLeds[6] = (Led *) ui->sample7Led;
    kitLeds[7] = (Led *) ui->sample8Led;

    on_mixerVolume1Slide_valueChanged(0);
    on_mixerVolume2Slide_valueChanged(0);
    on_mixerVolume3Slide_valueChanged(0);
    on_mixerVolume4Slide_valueChanged(0);
    on_mixerVolume5Slide_valueChanged(0);
    on_mixerVolume6Slide_valueChanged(0);
    on_mixerVolume7Slide_valueChanged(0);
    on_mixerVolume8Slide_valueChanged(0);

    on_mixerVolumeFMSlide_valueChanged(0);
    on_mixerVolumeKitSlide_valueChanged(0);
    on_mixerVolumeMasterSlide_valueChanged(0);

    setSampleSet(0);
}

MixingPage::~MixingPage()
{
    delete ui;
}


/*****************************************************************************/
void MixingPage::refreshMixingLeds()
{
    for (int c = 0; c < 10; c++){
        if (!mixingLeds[c]) continue;
        if (midiChannelsAct[c]) {
            mixingLeds[c]->luminosity = 0xF000;
            mixingLeds[c]->repaint();
        }else if (mixingLeds[c]->luminosity >= 0x1000) {
            mixingLeds[c]->luminosity -= 0x1000;
            mixingLeds[c]->repaint();
        }
    }
}


void MixingPage::refreshSampleLeds()
{
    int k = currentSet * 8;
    for (int c = 0; c < 8; c++){
        if (midiSamplesAct[k+c]) {
            kitLeds[c]->luminosity = 0xF000;
            kitLeds[c]->repaint();
        }else if (kitLeds[c]->luminosity >= 0x1000) {
            kitLeds[c]->luminosity -= 0x1000;
            kitLeds[c]->repaint();
        }
    }
}

/*****************************************************************************/
void MixingPage::setSampleSet(int set)
{
    currentSet = set;

    ui->kitSet1Push->setChecked(set == 0);
    ui->kitSet2Push->setChecked(set == 1);
    ui->kitSet3Push->setChecked(set == 2);
    ui->kitSet4Push->setChecked(set == 3);

    int k = currentSet * 8;
    ui->kitSample1Label->setText(Opa::kitSampleNames[k]);
    ui->kitSample2Label->setText(Opa::kitSampleNames[k+1]);
    ui->kitSample3Label->setText(Opa::kitSampleNames[k+2]);
    ui->kitSample4Label->setText(Opa::kitSampleNames[k+3]);
    ui->kitSample5Label->setText(Opa::kitSampleNames[k+4]);
    ui->kitSample6Label->setText(Opa::kitSampleNames[k+5]);
    ui->kitSample7Label->setText(Opa::kitSampleNames[k+6]);
    ui->kitSample8Label->setText(Opa::kitSampleNames[k+7]);
}

/*****************************************************************************/
void MixingPage::setContent(int program, const OpaProgramParams * params,  bool send)
{
    opa.setEnable(false);
    switch(program) {
    case 0:
        ui->mixerVolume1Slide->setValue(params->volume);
        ui->mixerPan1Dial->setValue(params->panning);
        ui->mixerMute1Push->setChecked(params->flags & OPA_PROGRAM_MUTED);
        ui->mixerSteal1Push->setChecked(params->flags & OPA_PROGRAM_STEALING);
        break;
    case 1:
        ui->mixerVolume2Slide->setValue(params->volume);
        ui->mixerPan2Dial->setValue(params->panning);
        ui->mixerMute2Push->setChecked(params->flags & OPA_PROGRAM_MUTED);
        ui->mixerSteal2Push->setChecked(params->flags & OPA_PROGRAM_STEALING);
        break;
    case 2:
        ui->mixerVolume3Slide->setValue(params->volume);
        ui->mixerPan3Dial->setValue(params->panning);
        ui->mixerMute3Push->setChecked(params->flags & OPA_PROGRAM_MUTED);
        ui->mixerSteal3Push->setChecked(params->flags & OPA_PROGRAM_STEALING);
        break;
    case 3:
        ui->mixerVolume4Slide->setValue(params->volume);
        ui->mixerPan4Dial->setValue(params->panning);
        ui->mixerMute4Push->setChecked(params->flags & OPA_PROGRAM_MUTED);
        ui->mixerSteal4Push->setChecked(params->flags & OPA_PROGRAM_STEALING);
        break;
    case 4:
        ui->mixerVolume5Slide->setValue(params->volume);
        ui->mixerPan5Dial->setValue(params->panning);
        ui->mixerMute5Push->setChecked(params->flags & OPA_PROGRAM_MUTED);
        ui->mixerSteal5Push->setChecked(params->flags & OPA_PROGRAM_STEALING);
        break;
    case 5:
        ui->mixerVolume6Slide->setValue(params->volume);
        ui->mixerPan6Dial->setValue(params->panning);
        ui->mixerMute6Push->setChecked(params->flags & OPA_PROGRAM_MUTED);
        ui->mixerSteal6Push->setChecked(params->flags & OPA_PROGRAM_STEALING);
        break;
    case 6:
        ui->mixerVolume7Slide->setValue(params->volume);
        ui->mixerPan7Dial->setValue(params->panning);
        ui->mixerMute7Push->setChecked(params->flags & OPA_PROGRAM_MUTED);
        ui->mixerSteal7Push->setChecked(params->flags & OPA_PROGRAM_STEALING);
        break;
    case 7:
        ui->mixerVolume8Slide->setValue(params->volume);
        ui->mixerPan8Dial->setValue(params->panning);
        ui->mixerMute8Push->setChecked(params->flags & OPA_PROGRAM_MUTED);
        ui->mixerSteal8Push->setChecked(params->flags & OPA_PROGRAM_STEALING);
        break;
    case 8:
        // TODO
        break;
    }

    opa.setEnable(true);

    if (send) {
        opa.paramWrite(program, OPA_CONFIG_VOLUME, params->volume);
        opa.paramWrite(program, OPA_CONFIG_PANNING, params->panning);
        opa.paramWrite(program, OPA_CONFIG_FLAGS, params->flags);
   }
}

void MixingPage::setGlobalsContent(const OpaGlobals * params, bool send)
{
    opa.setEnable(false);
    ui->mixerVolumeMasterSlide->setValue(params->volume);
    ui->mixerVolumeFMSlide->setValue(params->fmVolume);
    ui->mixerVolumeKitSlide->setValue(params->kitVolume);
    opa.setEnable(true);

    if (send) {
        opa.globalsParamWrite(OPA_GLOBAL_VOLUME, params->volume);
        opa.globalsParamWrite(OPA_GLOBAL_FMVOLUME, params->fmVolume);
        opa.globalsParamWrite(OPA_GLOBAL_KITVOLUME, params->kitVolume);
    }
}


/*****************************************************************************/
void MixingPage::on_mixerVolume1Slide_valueChanged(int value)
{
    opa.paramWrite(0, OPA_CONFIG_VOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolume1Edit->setText(volume);
}

void MixingPage::on_mixerVolume2Slide_valueChanged(int value)
{
    opa.paramWrite(1, OPA_CONFIG_VOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolume2Edit->setText(volume);
}

void MixingPage::on_mixerVolume3Slide_valueChanged(int value)
{
    opa.paramWrite(2, OPA_CONFIG_VOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolume3Edit->setText(volume);
}

void MixingPage::on_mixerVolume4Slide_valueChanged(int value)
{
    opa.paramWrite(3, OPA_CONFIG_VOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolume4Edit->setText(volume);
}

void MixingPage::on_mixerVolume5Slide_valueChanged(int value)
{
    opa.paramWrite(4, OPA_CONFIG_VOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolume5Edit->setText(volume);
}

void MixingPage::on_mixerVolume6Slide_valueChanged(int value)
{
    opa.paramWrite(5, OPA_CONFIG_VOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolume6Edit->setText(volume);
}

void MixingPage::on_mixerVolume7Slide_valueChanged(int value)
{
    opa.paramWrite(6, OPA_CONFIG_VOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolume7Edit->setText(volume);
}

void MixingPage::on_mixerVolume8Slide_valueChanged(int value)
{
    opa.paramWrite(7, OPA_CONFIG_VOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolume8Edit->setText(volume);
}

void MixingPage::on_mixerVolumeFMSlide_valueChanged(int value)
{
    opa.globalsParamWrite(OPA_GLOBAL_FMVOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolumeFMEdit->setText(volume);
}

void MixingPage::on_mixerVolumeKitSlide_valueChanged(int value)
{
    opa.globalsParamWrite(OPA_GLOBAL_KITVOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolumeKitEdit->setText(volume);
}

void MixingPage::on_mixerVolumeMasterSlide_valueChanged(int value)
{
    opa.globalsParamWrite(OPA_GLOBAL_VOLUME, value);
    QString volume;
    volume.setNum(value, 10);
    ui->mixerVolumeMasterEdit->setText(volume);
}

/*****************************************************************************/
void MixingPage::writeFlags(int program)
{
    int flags = 0;
    switch(program) {
        case 0:
            flags = (ui->mixerSteal1Push->isChecked() ? OPA_PROGRAM_STEALING : 0) |
                    (ui->mixerMute1Push->isChecked() ? OPA_PROGRAM_MUTED : 0);
            break;
        case 1:
            flags = (ui->mixerSteal2Push->isChecked() ? OPA_PROGRAM_STEALING : 0) |
                    (ui->mixerMute2Push->isChecked() ? OPA_PROGRAM_MUTED : 0);
            break;
        case 2:
            flags = (ui->mixerSteal3Push->isChecked() ? OPA_PROGRAM_STEALING : 0) |
                    (ui->mixerMute3Push->isChecked() ? OPA_PROGRAM_MUTED : 0);
            break;
        case 3:
            flags = (ui->mixerSteal4Push->isChecked() ? OPA_PROGRAM_STEALING : 0) |
                    (ui->mixerMute4Push->isChecked() ? OPA_PROGRAM_MUTED : 0);
            break;
        case 4:
            flags = (ui->mixerSteal5Push->isChecked() ? OPA_PROGRAM_STEALING : 0) |
                    (ui->mixerMute5Push->isChecked() ? OPA_PROGRAM_MUTED : 0);
            break;
        case 5:
            flags = (ui->mixerSteal6Push->isChecked() ? OPA_PROGRAM_STEALING : 0) |
                    (ui->mixerMute6Push->isChecked() ? OPA_PROGRAM_MUTED : 0);
            break;
        case 6:
            flags = (ui->mixerSteal7Push->isChecked() ? OPA_PROGRAM_STEALING : 0) |
                    (ui->mixerMute7Push->isChecked() ? OPA_PROGRAM_MUTED : 0);
            break;
        case 7:
            flags = (ui->mixerSteal8Push->isChecked() ? OPA_PROGRAM_STEALING : 0) |
                    (ui->mixerMute8Push->isChecked() ? OPA_PROGRAM_MUTED : 0);
            break;
        case 8:
            // TODO
            break;
    }
    opa.paramWrite(program, OPA_CONFIG_FLAGS, flags);
}

void MixingPage::writeGlobalFlags()
{
    int flags = 0;
    flags |= mainWindow->ui->memoryProtectionAction->isChecked() ? OPA_GLOBAL_PROTECT : 0;
    flags |= ui->mixerMuteFMPush->isChecked() ? OPA_GLOBAL_MUTEFM : 0;
    flags |= ui->mixerMuteKitPush->isChecked() ? OPA_GLOBAL_MUTEKIT : 0;

    opa.globalsParamWrite(OPA_GLOBAL_FLAGS, flags);
}

/*****************************************************************************/
void MixingPage::on_mixerPan1Dial_valueChanged(int value)
{
    opa.paramWrite(0, OPA_CONFIG_PANNING, value);
}

void MixingPage::on_mixerPan2Dial_valueChanged(int value)
{
    opa.paramWrite(1, OPA_CONFIG_PANNING, value);
}

void MixingPage::on_mixerPan3Dial_valueChanged(int value)
{
    opa.paramWrite(2, OPA_CONFIG_PANNING, value);
}

void MixingPage::on_mixerPan4Dial_valueChanged(int value)
{
    opa.paramWrite(3, OPA_CONFIG_PANNING, value);
}

void MixingPage::on_mixerPan5Dial_valueChanged(int value)
{
    opa.paramWrite(4, OPA_CONFIG_PANNING, value);
}

void MixingPage::on_mixerPan6Dial_valueChanged(int value)
{
    opa.paramWrite(5, OPA_CONFIG_PANNING, value);
}

void MixingPage::on_mixerPan7Dial_valueChanged(int value)
{
    opa.paramWrite(7, OPA_CONFIG_PANNING, value);
}

void MixingPage::on_mixerPan8Dial_valueChanged(int value)
{
    opa.paramWrite(8, OPA_CONFIG_PANNING, value);
}

/*****************************************************************************/
void MixingPage::on_mixerMute1Push_toggled(bool checked)
{
    writeFlags(0);
}

void MixingPage::on_mixerMute2Push_toggled(bool checked)
{
    writeFlags(1);
}

void MixingPage::on_mixerMute3Push_toggled(bool checked)
{
    writeFlags(2);
}

void MixingPage::on_mixerMute4Push_toggled(bool checked)
{
    writeFlags(3);
}

void MixingPage::on_mixerMute5Push_toggled(bool checked)
{
    writeFlags(4);
}

void MixingPage::on_mixerMute6Push_toggled(bool checked)
{
    writeFlags(5);
}

void MixingPage::on_mixerMute7Push_toggled(bool checked)
{
    writeFlags(6);
}

void MixingPage::on_mixerMute8Push_toggled(bool checked)
{
    writeFlags(7);
}

void MixingPage::on_mixerMuteFMPush_toggled(bool checked)
{
    writeGlobalFlags();
}

void MixingPage::on_mixerMuteKitPush_toggled(bool checked)
{
    writeGlobalFlags();
}

/*****************************************************************************/
void MixingPage::on_mixerSteal1Push_toggled(bool checked)
{
    writeFlags(0);
}

void MixingPage::on_mixerSteal2Push_toggled(bool checked)
{
    writeFlags(1);
}

void MixingPage::on_mixerSteal3Push_toggled(bool checked)
{
    writeFlags(2);
}

void MixingPage::on_mixerSteal4Push_toggled(bool checked)
{
    writeFlags(3);
}

void MixingPage::on_mixerSteal5Push_toggled(bool checked)
{
    writeFlags(4);
}

void MixingPage::on_mixerSteal6Push_toggled(bool checked)
{
    writeFlags(5);
}

void MixingPage::on_mixerSteal7Push_toggled(bool checked)
{
    writeFlags(6);
}

void MixingPage::on_mixerSteal8Push_toggled(bool checked)
{
    writeFlags(7);
}

/*****************************************************************************/
void MixingPage::on_kitVolume1Dial_valueChanged(int value)
{
    opa.kitParamWrite(0, OPA_KIT_VOLUME, value);
}

void MixingPage::on_kitVolume2Dial_valueChanged(int value)
{
    opa.kitParamWrite(1, OPA_KIT_VOLUME, value);
}

void MixingPage::on_kitVolume3Dial_valueChanged(int value)
{
    opa.kitParamWrite(2, OPA_KIT_VOLUME, value);
}

void MixingPage::on_kitVolume4Dial_valueChanged(int value)
{
    opa.kitParamWrite(3, OPA_KIT_VOLUME, value);
}

void MixingPage::on_kitVolume5Dial_valueChanged(int value)
{
    opa.kitParamWrite(4, OPA_KIT_VOLUME, value);
}

void MixingPage::on_kitVolume6Dial_valueChanged(int value)
{
    opa.kitParamWrite(5, OPA_KIT_VOLUME, value);
}

void MixingPage::on_kitVolume7Dial_valueChanged(int value)
{
    opa.kitParamWrite(6, OPA_KIT_VOLUME, value);
}

void MixingPage::on_kitVolume8Dial_valueChanged(int value)
{
    opa.kitParamWrite(7, OPA_KIT_VOLUME, value);
}

void MixingPage::on_kitPan1Dial_valueChanged(int value)
{
    opa.kitParamWrite(0, OPA_KIT_PANNING, value);
}

void MixingPage::on_kitPan2Dial_valueChanged(int value)
{
    opa.kitParamWrite(1, OPA_KIT_PANNING, value);
}

void MixingPage::on_kitPan3Dial_valueChanged(int value)
{
    opa.kitParamWrite(2, OPA_KIT_PANNING, value);
}

void MixingPage::on_kitPan4Dial_valueChanged(int value)
{
    opa.kitParamWrite(3, OPA_KIT_PANNING, value);
}

void MixingPage::on_kitPan5Dial_valueChanged(int value)
{
    opa.kitParamWrite(4, OPA_KIT_PANNING, value);
}

void MixingPage::on_kitPan6Dial_valueChanged(int value)
{
    opa.kitParamWrite(5, OPA_KIT_PANNING, value);
}

void MixingPage::on_kitPan7Dial_valueChanged(int value)
{
    opa.kitParamWrite(6, OPA_KIT_PANNING, value);
}

void MixingPage::on_kitPan8Dial_valueChanged(int value)
{
    opa.kitParamWrite(7, OPA_KIT_PANNING, value);
}

void MixingPage::on_kitDecay1Dial_valueChanged(int value)
{
    opa.kitParamWrite(0, OPA_KIT_DECAY, value);
}

void MixingPage::on_kitDecay2Dial_valueChanged(int value)
{
    opa.kitParamWrite(1, OPA_KIT_DECAY, value);
}

void MixingPage::on_kitDecay3Dial_valueChanged(int value)
{
    opa.kitParamWrite(2, OPA_KIT_DECAY, value);
}

void MixingPage::on_kitDecay4Dial_valueChanged(int value)
{
    opa.kitParamWrite(3, OPA_KIT_DECAY, value);
}

void MixingPage::on_kitDecay5Dial_valueChanged(int value)
{
    opa.kitParamWrite(4, OPA_KIT_DECAY, value);
}

void MixingPage::on_kitDecay6Dial_valueChanged(int value)
{
    opa.kitParamWrite(5, OPA_KIT_DECAY, value);
}

void MixingPage::on_kitDecay7Dial_valueChanged(int value)
{
    opa.kitParamWrite(6, OPA_KIT_DECAY, value);
}

void MixingPage::on_kitDecay8Dial_valueChanged(int value)
{
    opa.kitParamWrite(7, OPA_KIT_DECAY, value);
}

/*****************************************************************************/
void MixingPage::on_kitSet1Push_clicked()
{
    setSampleSet(0);
}

void MixingPage::on_kitSet2Push_clicked()
{
    setSampleSet(1);
}

void MixingPage::on_kitSet3Push_clicked()
{
    setSampleSet(2);
}

void MixingPage::on_kitSet4Push_clicked()
{
    setSampleSet(3);
}

/*****************************************************************************/
void MixingPage::on_mixerTuneDial_valueChanged(int value)
{
    QString semi;
    semi.setNum(value, 10);
    ui->mixerTuneEditLabel->setText(semi + " semi");
    opa.globalsParamWrite(OPA_GLOBAL_COARSE, value);
}

