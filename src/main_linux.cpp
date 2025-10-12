#include <QApplication>
#include "main_window.hpp"

// mostly used for linux
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
