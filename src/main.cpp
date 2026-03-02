#include <QApplication>
#include <QFile>
#include "main_window.hpp"
#include "resources/style_loader.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("yt-dlp-gui");
    QCoreApplication::setApplicationName("yt-dlp-gui");

    app.setStyleSheet(loadStyleSheet(Theme::Dark));
    app.setWindowIcon(QIcon(":/icons/taskbar_icon.png"));

    MainWindow w;
    w.show();

    return app.exec();
}
