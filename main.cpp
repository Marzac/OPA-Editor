#include "mainwindow.h"
#include <QApplication>

/*
#ifdef _WIN32
#include "objbase.h"
#endif
*/
int main(int argc, char *argv[])
{
/*
#ifdef _WIN32
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
*/
    QApplication a(argc, argv);
    MainWindow mw;
    mw.show();

    return a.exec();

/*
#ifdef _WIN32
    CoUninitialize();
#endif
*/
}
