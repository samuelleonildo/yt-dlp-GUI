#include "main_window.hpp"

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QTextCursor>
#include <QScrollBar>
#include <QRegularExpression>
#include <QButtonGroup>
#include <QString>
#include <QProcess>
#include <QComboBox>

#ifdef Q_OS_WIN
#include <windows.h>
#endif


// ---------- Helper functions ----------
void MainWindow::sanitizeFilename(QString &s)
{
    static const QRegularExpression re(R"([\\/:\*\?\"<>\|])");
    s.replace(re, "_");
    s = s.trimmed();
    if (s.size() > 100) s = s.left(100);
}

bool MainWindow::isPlaylistUrl(const QString &url)
{
    return url.contains("list=") || url.contains("playlist?");
}

// ---------- MainWindow ----------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // --- mode selector ---
    QLabel *lblMode = new QLabel("Mode:");
    cbMode = new QComboBox;
    cbMode->addItems({"video", "audio"});
    cbMode->setObjectName("cbMode");


    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->addWidget(lblMode);
    modeLayout->addWidget(cbMode);
    modeLayout->addStretch();

    // --- format selector ---
    QLabel *lblFormat = new QLabel("Format:");
    cbFormat = new QComboBox;
    cbFormat->addItems({"mp4", "mkv"}); 
    cbFormat->setObjectName("cbFormat");

    QHBoxLayout *formatLayout = new QHBoxLayout;
    formatLayout->addWidget(lblFormat);
    formatLayout->addWidget(cbFormat);
    formatLayout->addStretch();

    // --- video quality ---
    QLabel *lblVideoQuality = new QLabel("Video Quality:");

    QStringList videoQualities = {"240p", "360p", "480p", "720p", "1080p", "4K", "Best"};
    QHBoxLayout *videoQualityLayout = new QHBoxLayout;
    QButtonGroup *videoGroup = new QButtonGroup(this);
    videoGroup->setExclusive(true);

    for (const QString &q : videoQualities)
    {
        QPushButton *btn = new QPushButton(q);
        btn->setCheckable(true);
        btn->setStyleSheet(
            "QPushButton { padding: 6px 10px; border: 1px solid gray; border-radius: 6px; }"
            "QPushButton:checked { background-color: #0078D7; color: white; font-weight: bold; }"
        );
        videoGroup->addButton(btn);
        videoQualityLayout->addWidget(btn);
    }

    // best as default quality
    videoGroup->buttons().last()->setChecked(true);

    // --- audio quality ---
    QLabel *lblAudioQuality = new QLabel("Audio Quality:");

    QStringList audioQualities = {"128k", "192k", "256k", "320k", "Best"};
    QHBoxLayout *audioQualityLayout = new QHBoxLayout;
    QButtonGroup *audioGroup = new QButtonGroup(this);
    audioGroup->setExclusive(true);

    for (const QString &q : audioQualities)
    {
        QPushButton *btn = new QPushButton(q);
        btn->setCheckable(true);
        btn->setStyleSheet(
            "QPushButton { padding: 6px 10px; border: 1px solid gray; border-radius: 6px; }"
            "QPushButton:checked { background-color: #2E8B57; color: white; font-weight: bold; }"
        );
        audioGroup->addButton(btn);
        audioQualityLayout->addWidget(btn);
    }

    audioGroup->buttons().last()->setChecked(true);

    // --- saving pointers (for future uses) ---
    this->videoQualityGroup = videoGroup;
    this->audioQualityGroup = audioGroup;

    // --- URL ---
    QLabel *lblUrl = new QLabel("URL:");
    leUrl = new QLineEdit;
    leUrl->setPlaceholderText("https://...");

    // --- path ---
    QLabel *lblPath = new QLabel("Save to:");
    lePath = new QLineEdit;
    lePath->setPlaceholderText("/home/user/Videos");
    QPushButton *btnBrowse = new QPushButton("Choose...");
    connect(btnBrowse, &QPushButton::clicked, this, &MainWindow::chooseDir);

    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(lblPath);
    pathLayout->addWidget(lePath);
    pathLayout->addWidget(btnBrowse);

    // --- custom name ---
    QLabel *lblCustom = new QLabel("Custom name (optional):");
    leCustom = new QLineEdit;
    leCustom->setPlaceholderText("without extension and special characters");

    // --- main buttons ---
    QPushButton *btnDownload = new QPushButton("Download");
    QPushButton *btnHelp = new QPushButton("Help");
    QPushButton *btnVersion = new QPushButton("Version");

    connect(btnDownload, &QPushButton::clicked, this, &MainWindow::startDownload);
    connect(btnHelp, &QPushButton::clicked, this, &MainWindow::showHelp);
    connect(btnVersion, &QPushButton::clicked, this, &MainWindow::showVersion);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(btnDownload);
    buttonsLayout->addWidget(btnHelp);
    buttonsLayout->addWidget(btnVersion);
    buttonsLayout->addStretch();

    // --- log ---
    log = new QTextEdit;
    log->setReadOnly(true);
    log->setMinimumHeight(200);

    // --- main layout ---
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(modeLayout);
    mainLayout->addLayout(formatLayout);

    mainLayout->addWidget(lblVideoQuality);
    mainLayout->addLayout(videoQualityLayout);
    mainLayout->addWidget(lblAudioQuality);
    mainLayout->addLayout(audioQualityLayout);

    mainLayout->addWidget(lblUrl);
    mainLayout->addWidget(leUrl);
    mainLayout->addLayout(pathLayout);
    mainLayout->addWidget(lblCustom);
    mainLayout->addWidget(leCustom);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(new QLabel("Console output:"));
    mainLayout->addWidget(log);

    central->setLayout(mainLayout);

    setWindowTitle("yt-dlp helper (Qt)");
    resize(1280, 720);
    setFixedSize(size());

    // quality swap
    lblAudioQuality->setVisible(false);
    for (auto *btn : audioGroup->buttons())
        btn->setVisible(false);

    cbFormat->clear();
    cbFormat->addItems({"mp4", "mkv"});

    connect(cbMode, &QComboBox::currentTextChanged, this, [=](const QString &mode) {
        QString initialMode = cbMode->currentText();

        lblAudioQuality->setVisible(initialMode == "audio");
        for (auto *btn : audioGroup->buttons())
            btn->setVisible(initialMode == "audio");

        lblVideoQuality->setVisible(initialMode == "video");
        for (auto *btn : videoGroup->buttons())
            btn->setVisible(initialMode == "video");

        cbFormat->clear();
        if (initialMode == "audio")
            cbFormat->addItems({"mp3", "opus"});
        else
            cbFormat->addItems({"mp4", "mkv"});
    });


    // --- process connections ---
    connect(&proc, &QProcess::readyReadStandardOutput, this, &MainWindow::readStdout);
    connect(&proc, &QProcess::readyReadStandardError, this, &MainWindow::readStderr);
    connect(&proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::processFinished);

    // --- deps paths ---
    depsPath = QDir(qApp->applicationDirPath()).filePath("deps");
#ifdef Q_OS_WIN
    ytDlpPath = QDir(depsPath).filePath("yt-dlp.exe");
    ffmpegPath = QDir(depsPath).filePath("ffmpeg.exe");
#else
    ytDlpPath = QDir(depsPath).filePath("yt-dlp");
    ffmpegPath = QDir(depsPath).filePath("ffmpeg");
#endif
}


// ---------- slots implementation ----------
void MainWindow::chooseDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Choose directory", lePath->text());
    if (!dir.isEmpty()) lePath->setText(dir);
}

void MainWindow::showHelp()
{
    QMessageBox::information(this, "Help",
        "Usage:\n  mode: video or audio\n  URL: must start with http:// or https://\n\nThis GUI is a helper wrapper around yt-dlp and ffmpeg.\nMake sure yt-dlp and ffmpeg are in release/deps.");
}

void MainWindow::showVersion()
{
    QMessageBox::information(this, "Version", "yt-dlp helper Qt v1.0");
}


void MainWindow::startDownload()
{
    // getting widgets
    QString mode = cbMode ? cbMode->currentText() : "video";
    QString format = cbFormat ? cbFormat->currentText() : "mp4";

    QString url = leUrl->text().trimmed();
    QString dir = lePath->text().trimmed();
    QString custom = leCustom->text().trimmed();

    // --- validations ---
    if (url.isEmpty() || (!url.startsWith("http://") && !url.startsWith("https://")))
    {
        QMessageBox::warning(this, "Error", "Invalid URL. Must start with http:// or https://");
        return;
    }
    if (dir.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Choose a destination directory.");
        return;
    }

    bool is_playlist = isPlaylistUrl(url);
    if (is_playlist)
    {
        log->append("Warning: playlist detected — using yt-dlp's auto name.");
        custom.clear();
        leCustom->clear();
    }

    if (!custom.isEmpty())
    {
        sanitizeFilename(custom);
        if (custom.isEmpty())
        {
            QMessageBox::warning(this, "Warning", "Custom name is empty after sanitization.");
            return;
        }
    }

    QDir qd(dir);
    if (!qd.exists() && !qd.mkpath(dir))
    {
        QMessageBox::critical(this, "Error", "Failed to create the directory: " + dir);
        return;
    }

    // --- check dependencies ---
    log->append("Checking dependencies: yt-dlp and ffmpeg...");

    QProcess check;
    check.start(ytDlpPath, {"--version"});
    if (!check.waitForFinished(3000) || check.exitCode() != 0)
    {
        QMessageBox::critical(this, "Error", "yt-dlp not found in release/deps.");
        return;
    }

    check.start(ffmpegPath, {"-version"});
    if (!check.waitForFinished(3000) || check.exitCode() != 0)
    {
        QMessageBox::critical(this, "Error", "ffmpeg not found in release/deps.");
        return;
    }

    // --- output path ---
    QString outputTemplate;
    if (!custom.isEmpty())
        outputTemplate = dir + "/" + custom + ".%(ext)s";
    else if (is_playlist)
        outputTemplate = dir + "/%(playlist)s/%(playlist_index)03d - %(title)s.%(ext)s";
    else
        outputTemplate = dir + "/%(title)s.%(ext)s";

    // --- base arguments ---
    QStringList args;
    args << "--ffmpeg-location" << ffmpegPath;

    QString videoQuality = videoQualityGroup->checkedButton()->text();
    QString audioQuality = audioQualityGroup->checkedButton()->text();

    // --- audio mode ---
    if (mode == "audio")
    {
        args << "-x";
        args << "--audio-format" << format;

        if (audioQuality != "Best")
            args << "--audio-quality" << audioQuality;
    }
    else // --- video mode ---
    {
        QString qualityValue = videoQuality;
        qualityValue.remove("p"); // ex: "720p" -> "720"

        if (videoQuality == "Best" || videoQuality == "4K")
            qualityValue = "4320"; // max limit

        QString formatArg = QString("bestvideo[height<=%1]+bestaudio/best").arg(qualityValue);
        args << "-f" << formatArg;
        args << "--merge-output-format" << format;
    }

    args << url << "-o" << outputTemplate;

    // --- starting process ---
    log->clear();
    log->append("Running command: " + ytDlpPath + " " + args.join(" "));

    proc.start(ytDlpPath, args);
    if (!proc.waitForStarted(3000))
    {
        QMessageBox::critical(this, "Error", "Failed to execute yt-dlp.");
        return;
    }

    btnIsRunning = true;
}



void MainWindow::readStdout()
{
    QByteArray b = proc.readAllStandardOutput();
    if (!b.isEmpty())
    {
        log->moveCursor(QTextCursor::End);
        log->insertPlainText(QString::fromLocal8Bit(b));
        log->verticalScrollBar()->setValue(log->verticalScrollBar()->maximum());
    }
}

void MainWindow::readStderr()
{
    QByteArray b = proc.readAllStandardError();
    if (!b.isEmpty())
    {
        log->moveCursor(QTextCursor::End);
        log->insertPlainText(QString::fromLocal8Bit(b));
        log->verticalScrollBar()->setValue(log->verticalScrollBar()->maximum());
    }
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus)
{
    btnIsRunning = false;
    if (exitCode == 0)
    {
        log->append("\nCommand executed successfully.");
        QMessageBox::information(this, "Completed", "Download completed successfully.");
    }
    else
    {
        log->append(QString("\nyt-dlp exited with code %1").arg(exitCode));
        QMessageBox::warning(this, "Error", QString("yt-dlp finished with exit code %1").arg(exitCode));
    }
}
