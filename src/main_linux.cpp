#include <QApplication>
#include <QFile>
#include "main_window.hpp"
#include "resources/style_loader.hpp"

// mostly used for linux
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // style
    app.setStyleSheet(loadStyleSheet(Theme::Dark));
    app.setWindowIcon(QIcon(":/icons/taskbar_icon.png"));

    MainWindow w;
    w.show();

    return app.exec();
}
