#ifndef PROGRAMSPAGE_H
#define PROGRAMSPAGE_H

#include <QWidget>

#include "operatorwidget.h"
#include "programwidget.h"
#include "led.h"

namespace Ui {
class ProgramsPage;
}

class ProgramsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProgramsPage(QWidget *parent = 0);
    ~ProgramsPage();

    void setProgram(int program);
    void setAlgorithm(int a);

    void refreshProgramLeds();

    Led * programLeds[8];

private slots:
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

private:
    Ui::ProgramsPage *ui;
};

#endif // PROGRAMSPAGE_H
