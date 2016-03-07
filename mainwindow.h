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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QLabel>
#include <QTimer>

#include "operatorwidget.h"
#include "programwidget.h"
#include "midi/midiin.h"
#include "led.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void devicesEnumerate();

    void programRead(int program);
    void programWrite(int program);
    void globalRead();
    void globalWrite();

    void paramBulkRead(int program, int firstParam, uint8_t * paramBuffer, int noParams);

    void refresh();
    static MainWindow * getInstance();

    void setProgram(int i);
    void setAlgorithm(int a);
    int getAlgorithm() {return algorithm;}
    bool getMemoryProtection();

private slots:
    void comTimer_timeout();
    void UITimer_timeout();

    void on_helpMenu_triggered(QAction * action);
    void on_fileMenu_triggered(QAction * action);
    void on_deviceMenu_triggered(QAction * action);
    void on_connectionMenu_triggered(QAction * action);

    void on_i1Push_clicked();
    void on_i2Push_clicked();
    void on_i3Push_clicked();
    void on_i4Push_clicked();
    void on_i5Push_clicked();
    void on_i6Push_clicked();
    void on_i7Push_clicked();
    void on_i8Push_clicked();

    void on_algo1Push_clicked();
    void on_algo2Push_clicked();
    void on_algo3Push_clicked();
    void on_algo4Push_clicked();
    void on_algo5Push_clicked();
    void on_algo6Push_clicked();
    void on_algo7Push_clicked();
    void on_algo8Push_clicked();
    void on_algo9Push_clicked();
    void on_algo10Push_clicked();
    void on_algo11Push_clicked();
    void on_algo12Push_clicked();
    void on_algo13Push_clicked();

    void on_masterTuneDial_valueChanged(int value);
    void on_masterVolumeDial_valueChanged(int value);

private:
    void writeFlags();
    void setFlags(int flags);

    void refreshProgramLeds();

private:
    Ui::MainWindow *ui;

    QTimer * comTimer;
    QTimer * UITimer;
    QLabel * statusLabel;

    Led * programLeds[8];
    int programIndex;
    int algorithm;

    OpaProgram programBuffer;
    OpaGlobals globalsBuffer;
    bool waitforGlobals;
    bool waitforProgram;
    bool waitforParam;

    bool needGlobalsRefresh;
    bool needProgramRefresh;

    MidiIn * midiIn;
    static void midiInCallback(uint8_t msg[]);
    static void drumMap(uint8_t msg[]);

    static int midiBendRange;
    static bool midiChannelsAct[16];

    static MainWindow * instance;
};

#endif // MAINWINDOW_H
