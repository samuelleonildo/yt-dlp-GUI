#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "./resources/style_loader.hpp"
#include <QMainWindow>
#include <QRadioButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QProcess>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    Theme current_theme = Theme::Dark;

    // widgets
    QPushButton* dark_mode;
    QPushButton* light_mode;

    QComboBox* cbCookies;
    QComboBox* cbFormat;
    QComboBox* cbMode;

    QButtonGroup* videoQualityGroup;
    QButtonGroup* audioQualityGroup;

    QLineEdit* leUrl;
    QLineEdit* lePath;
    QLineEdit* leCustom;
    QTextEdit* log;

    QProcess proc;
    bool btnIsRunning = false;

    // paths
    QString depsPath;
    QString ytDlpPath;
    QString ffmpegPath;

    // funcs
    void switchTheme(Theme theme);
    void applyInterFont();
    void chooseDir();
    void showHelp();
    void cancelDownload();
    void startDownload();
    void readStdout();
    void readStderr();
    void processFinished(int exitCode, QProcess::ExitStatus status);

    static void sanitizeFilename(QString &s);
    static bool isPlaylistUrl(const QString &url);
};

#endif // MAIN_WINDOW_HPP
