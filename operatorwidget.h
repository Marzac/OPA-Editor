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

#ifndef OPERATORWIDGET_H
#define OPERATORWIDGET_H

#include <QWidget>
#include "opa.h"

namespace Ui {
class OperatorWidget;
}

class OperatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OperatorWidget(QWidget *parent = 0);
    ~OperatorWidget();

    void setOperator(int index, bool feedback);
    void setProgram(int program);

    void setContent(const OpaOperatorParams * params, bool send);
    void getContent(OpaOperatorParams * params);

    void update();

private:
    void writeFlags();
    void setFlags(int flags);
    int getFlags();

    void updateEnvelope();
    int getParamIndex(int p);

private slots:
    void on_absoluteButton_clicked(bool checked);

    void on_coarseDial_valueChanged(int value);
    void on_fineDial_valueChanged(int value);

    void on_attackDial_valueChanged(int value);
    void on_decayDial_valueChanged(int value);
    void on_sustainDial_valueChanged(int value);
    void on_initDial_valueChanged(int value);
    void on_releaseDial_valueChanged(int value);

    void on_LFOSpeedDial_valueChanged(int value);
    void on_LFOAmountDial_valueChanged(int value);
    void on_feedbackDial_valueChanged(int value);

    void on_muteButton_clicked(bool checked);
    void on_volumeSlide_valueChanged(int value);

    void on_trackSoftHighButton_clicked();
    void on_trackHardHighButton_clicked();
    void on_trackSoftLowButton_clicked();
    void on_trackHardLowButton_clicked();

    void on_coarseLine_editingFinished();
    void on_fineLine_editingFinished();
    void on_volumeLine_editingFinished();


private:
    int operatorIndex;
    int programIndex;

private:
    Ui::OperatorWidget *ui;

};

#endif // OPERATORWIDGET_H
