#include <windows.h>
#include <QApplication>
#include "main_window.hpp"

// mostly used for Windows
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    int argc = 0;
    char **argv = nullptr;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
