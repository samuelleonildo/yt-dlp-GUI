#include <windows.h>
#include <QFile>
#include <QApplication>
#include "main_window.hpp"
#include "resources/style_loader.hpp"

// mostly used for Windows
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    int argc = 0;
    char **argv = nullptr;

    QApplication app(argc, argv);

    // // style
    app.setStyleSheet(loadStyleSheet(Theme::Dark));
    app.setWindowIcon(QIcon(":/icons/taskbar_icon.png"));

    MainWindow w;
    w.show();

    return app.exec();
}
