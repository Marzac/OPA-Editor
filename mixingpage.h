#ifndef MIXINGPAGE_H
#define MIXINGPAGE_H

#include <QWidget>

#include "led.h"
#include "opa.h"

namespace Ui {
class MixingPage;
}

class MixingPage : public QWidget
{
    Q_OBJECT

public:
    MixingPage(QWidget *parent = 0);
    ~MixingPage();

    void setSampleSet(int set);
    void setContent(int program, const OpaProgramParams * params, bool send);
    void setGlobalsContent(const OpaGlobals * params, bool send);

    void writeFlags(int program);
    void writeGlobalFlags();

    void refreshMixingLeds();
    void refreshSampleLeds();

private slots:
    void on_mixerVolume1Slide_valueChanged(int value);
    void on_mixerVolume2Slide_valueChanged(int value);
    void on_mixerVolume3Slide_valueChanged(int value);
    void on_mixerVolume4Slide_valueChanged(int value);
    void on_mixerVolume5Slide_valueChanged(int value);
    void on_mixerVolume6Slide_valueChanged(int value);
    void on_mixerVolume7Slide_valueChanged(int value);
    void on_mixerVolume8Slide_valueChanged(int value);
    void on_mixerVolumeFMSlide_valueChanged(int value);
    void on_mixerVolumeKitSlide_valueChanged(int value);
    void on_mixerVolumeMasterSlide_valueChanged(int value);

    void on_mixerPan1Dial_valueChanged(int value);
    void on_mixerPan2Dial_valueChanged(int value);
    void on_mixerPan3Dial_valueChanged(int value);
    void on_mixerPan4Dial_valueChanged(int value);
    void on_mixerPan5Dial_valueChanged(int value);
    void on_mixerPan6Dial_valueChanged(int value);
    void on_mixerPan7Dial_valueChanged(int value);
    void on_mixerPan8Dial_valueChanged(int value);

    void on_mixerMute1Push_toggled(bool checked);
    void on_mixerMute2Push_toggled(bool checked);
    void on_mixerMute3Push_toggled(bool checked);
    void on_mixerMute4Push_toggled(bool checked);
    void on_mixerMute5Push_toggled(bool checked);
    void on_mixerMute6Push_toggled(bool checked);
    void on_mixerMute7Push_toggled(bool checked);
    void on_mixerMute8Push_toggled(bool checked);
    void on_mixerMuteFMPush_toggled(bool checked);
    void on_mixerMuteKitPush_toggled(bool checked);

    void on_mixerSteal1Push_toggled(bool checked);
    void on_mixerSteal2Push_toggled(bool checked);
    void on_mixerSteal3Push_toggled(bool checked);
    void on_mixerSteal4Push_toggled(bool checked);
    void on_mixerSteal5Push_toggled(bool checked);
    void on_mixerSteal6Push_toggled(bool checked);
    void on_mixerSteal7Push_toggled(bool checked);
    void on_mixerSteal8Push_toggled(bool checked);

    void on_kitVolume1Dial_valueChanged(int value);
    void on_kitVolume2Dial_valueChanged(int value);
    void on_kitVolume3Dial_valueChanged(int value);
    void on_kitVolume4Dial_valueChanged(int value);
    void on_kitVolume5Dial_valueChanged(int value);
    void on_kitVolume6Dial_valueChanged(int value);
    void on_kitVolume7Dial_valueChanged(int value);
    void on_kitVolume8Dial_valueChanged(int value);

    void on_kitPan1Dial_valueChanged(int value);
    void on_kitPan2Dial_valueChanged(int value);
    void on_kitPan3Dial_valueChanged(int value);
    void on_kitPan4Dial_valueChanged(int value);
    void on_kitPan5Dial_valueChanged(int value);
    void on_kitPan6Dial_valueChanged(int value);
    void on_kitPan7Dial_valueChanged(int value);
    void on_kitPan8Dial_valueChanged(int value);

    void on_kitDecay1Dial_valueChanged(int value);
    void on_kitDecay2Dial_valueChanged(int value);
    void on_kitDecay3Dial_valueChanged(int value);
    void on_kitDecay4Dial_valueChanged(int value);
    void on_kitDecay5Dial_valueChanged(int value);
    void on_kitDecay6Dial_valueChanged(int value);
    void on_kitDecay7Dial_valueChanged(int value);
    void on_kitDecay8Dial_valueChanged(int value);

    void on_kitSet1Push_clicked();
    void on_kitSet2Push_clicked();
    void on_kitSet3Push_clicked();
    void on_kitSet4Push_clicked();

    void on_mixerTuneDial_valueChanged(int value);

private:
    int currentSet;
    Led * mixingLeds[10];
    Led * kitLeds[8];
    Ui::MixingPage *ui;
};

#endif // MIXINGPAGE_H
