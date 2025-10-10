#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QRadioButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QProcess>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    // widgets
    QComboBox *cbFormat;
    QComboBox *cbMode;

    QButtonGroup *formatGroup;
    QRadioButton *rbOpus;
    QRadioButton *rbMp3;
    QRadioButton *rbMkv;
    QRadioButton *rbMp4;

    QButtonGroup *videoQualityGroup;
    QButtonGroup *audioQualityGroup;

    QLineEdit *leUrl;
    QLineEdit *lePath;
    QLineEdit *leCustom;
    QTextEdit *log;

    QProcess proc;
    bool btnIsRunning = false;

    // paths
    QString depsPath;
    QString ytDlpPath;
    QString ffmpegPath;


    // funcs
    void chooseDir();
    void showHelp();
    void showVersion();
    void startDownload();
    void readStdout();
    void readStderr();
    void processFinished(int exitCode, QProcess::ExitStatus status);

    static void sanitizeFilename(QString &s);
    static bool isPlaylistUrl(const QString &url);
};

#endif // MAIN_WINDOW_HPP
