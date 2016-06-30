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

#ifndef PROGRAMWIDGET_H
#define PROGRAMWIDGET_H

#include <QWidget>
#include "opa.h"

namespace Ui {
class ProgramWidget;
}

class ProgramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgramWidget(QWidget *parent = 0);
    ~ProgramWidget();

    void setProgramIndex(int index);
    void setContent(const OpaProgramParams * params,  bool send);
    void getContent(OpaProgramParams * params);

    void updateUI();

private slots:
    void on_volumeDial_valueChanged(int value);
    void on_panningDial_valueChanged(int value);

    void on_initButton_clicked();
    void on_openButton_clicked();
    void on_saveButton_clicked();

    void on_loadButton_clicked();
    void on_storeButton_clicked();

    void on_nameLine_editingFinished();
    void on_slotSpin_valueChanged(int value);
    void on_stealingButton_clicked();
    void on_muteButton_clicked();

private:
    int programIndex;
    OpaProgram intProgramBuffer;
    bool waitforIntProgram;

private:
    void writeFlags();
    void setFlags(int flags);
    int  getFlags();

private:
    Ui::ProgramWidget *ui;

};

#endif // PROGRAMWIDGET_H
