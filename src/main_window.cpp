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
#include <QListView>
#include <QFontDatabase>


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

void MainWindow::applyInterFont(void)
{
    // loads font
    int fontId = QFontDatabase::addApplicationFont(":/fonts/Inter_Light.ttf");
    QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);

    // defines it for the entire application
    QFont interLight(family, 16);
    this->setFont(interLight);
}

void MainWindow::switchTheme(Theme theme)
{
    current_theme = theme;
    qApp->setStyleSheet(loadStyleSheet(theme));

    if (current_theme == Theme::Dark)
    {
        dark_mode->setIcon(QIcon(":/icons/moon_dark.png"));
        light_mode->setIcon(QIcon(":/icons/sun_dark.png"));
    }
    else
    {
        dark_mode->setIcon(QIcon(":/icons/moon_light.png"));
        light_mode->setIcon(QIcon(":/icons/sun_light.png"));
    }
}

// ---------- MainWindow ----------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("yt-dlp-GUI");
    resize(1280, 720);
    setFixedSize(size());

    this->setObjectName("MainWindow");
    this->applyInterFont();

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    central->setObjectName("Central");

    // --- URL ---
    QLabel *lblUrl = new QLabel("URL");
    lblUrl->setObjectName("Label");
    leUrl = new QLineEdit;
    leUrl->setPlaceholderText("https://...");
    leUrl->setFixedWidth(750);
    leUrl->setAlignment(Qt::AlignLeft);

    // --- dark/light mode ---
    QHBoxLayout *colors_mode_layout = new QHBoxLayout;
    colors_mode_layout->setAlignment(Qt::AlignRight);
    colors_mode_layout->setSpacing(10);
    QButtonGroup *colors_group = new QButtonGroup(this);
    colors_group->setExclusive(true);

    dark_mode = new QPushButton;
    dark_mode->setCheckable(true);
    dark_mode->setChecked(true);
    dark_mode->setFixedSize(35, 35);
    dark_mode->setObjectName("ColorModeButton");
    dark_mode->setIcon(QIcon(":/icons/moon_dark.png"));
    dark_mode->setIconSize(QSize(18, 18));
    colors_group->addButton(dark_mode);
    colors_mode_layout->addWidget(dark_mode);

    light_mode = new QPushButton;
    light_mode->setCheckable(true);
    light_mode->setChecked(false);
    light_mode->setFixedSize(35, 35);
    light_mode->setObjectName("ColorModeButton");
    light_mode->setIcon(QIcon(":/icons/sun_dark.png"));
    light_mode->setIconSize(QSize(18, 18));
    colors_group->addButton(light_mode);
    colors_mode_layout->addWidget(light_mode);

    connect(dark_mode, &QPushButton::clicked, this, [=]() {
        dark_mode->setChecked(true);
        light_mode->setChecked(false);
        switchTheme(Theme::Dark);
    });
    connect(light_mode, &QPushButton::clicked, this, [=]() {
        light_mode->setChecked(true);
        dark_mode->setChecked(false);
        switchTheme(Theme::Light);
    });

    QHBoxLayout* leUrl_and_color_mode = new QHBoxLayout;
    leUrl_and_color_mode->addWidget(leUrl);
    leUrl_and_color_mode->addStretch();
    leUrl_and_color_mode->addLayout(colors_mode_layout);

    QVBoxLayout* url_layout = new QVBoxLayout;
    url_layout->addWidget(lblUrl);
    url_layout->addLayout(leUrl_and_color_mode);
    url_layout->setSpacing(10);
    url_layout->setAlignment(Qt::AlignLeft);


    // --- mode and format selector ---
    QLabel *lblMode = new QLabel("Download");
    lblMode->setObjectName("Label");
    cbMode = new QComboBox;
    cbMode->addItems({"video", "audio"});
    cbMode->setFixedSize(240, 35);
    cbMode->setView(new QListView);

    cbFormat = new QComboBox;
    cbFormat->addItems({"mp4", "mkv"}); 
    cbFormat->setFixedSize(195, 35);
    cbFormat->setView(new QListView);

    QHBoxLayout *mode_and_format_layout = new QHBoxLayout;
    mode_and_format_layout->addWidget(cbMode);
    mode_and_format_layout->addWidget(cbFormat);
    mode_and_format_layout->setSpacing(10);

    QVBoxLayout *main_mode_layout = new QVBoxLayout;
    main_mode_layout->addWidget(lblMode);
    main_mode_layout->addLayout(mode_and_format_layout);
    main_mode_layout->setSpacing(10);

    // --- custom name ---
    QLabel *lblCustom = new QLabel("Custom name (optional):");
    lblCustom->setObjectName("Label");
    leCustom = new QLineEdit;
    leCustom->setPlaceholderText("no especial characters nor extension");
    leCustom->setFixedSize(445, 35);

    QVBoxLayout *customNameLayout = new QVBoxLayout;
    customNameLayout->addWidget(lblCustom);
    customNameLayout->addWidget(leCustom);
    customNameLayout->setSpacing(10);

    // --- download and custom name ---
    QHBoxLayout *download_and_custom_name_layout = new QHBoxLayout;
    download_and_custom_name_layout->addLayout(main_mode_layout);
    download_and_custom_name_layout->addLayout(customNameLayout);
    download_and_custom_name_layout->setSpacing(50);
    download_and_custom_name_layout->setAlignment(Qt::AlignLeft);


    // --- video quality ---
    QStringList videoQualities = {"240p", "360p", "480p", "720p", "1080p", "4K", "Best"};
    QHBoxLayout *videoQualityLayout = new QHBoxLayout;
    videoQualityLayout->setAlignment(Qt::AlignLeft);
    videoQualityLayout->setSpacing(30);
    videoQualityLayout->setContentsMargins(0, 10, 0, 10);
    QButtonGroup *videoGroup = new QButtonGroup(this);
    videoGroup->setExclusive(true);

    for (const QString &q : videoQualities)
    {
        QPushButton *btn = new QPushButton(q);
        btn->setCheckable(true);
        btn->setObjectName("QualityButton");
        btn->setFixedSize(130, 40);
        videoGroup->addButton(btn);
        videoQualityLayout->addWidget(btn);
    }

    // best as default quality
    videoGroup->buttons().last()->setChecked(true);


    // --- audio quality ---
    QStringList audioQualities = {"128k", "192k", "256k", "320k", "Best"};
    QHBoxLayout *audioQualityLayout = new QHBoxLayout;
    audioQualityLayout->setAlignment(Qt::AlignLeft);
    audioQualityLayout->setSpacing(30);
    audioQualityLayout->setContentsMargins(0, 10, 0, 10);
    QButtonGroup *audioGroup = new QButtonGroup(this);
    audioGroup->setExclusive(true);

    for (const QString &q : audioQualities)
    {
        QPushButton *btn = new QPushButton(q);
        btn->setCheckable(true);
        btn->setObjectName("QualityButton");
        btn->setFixedSize(130, 40);
        audioGroup->addButton(btn);
        audioQualityLayout->addWidget(btn);
    }

    audioGroup->buttons().last()->setChecked(true);

    // --- saving pointers (for future uses) ---
    this->videoQualityGroup = videoGroup;
    this->audioQualityGroup = audioGroup;


    // --- path ---
    QLabel *lblPath = new QLabel("Save to");
    lblPath->setObjectName("Label");
    lePath = new QLineEdit;
    lePath->setPlaceholderText("/home/user/Videos");
    lePath->setFixedSize(345, 35);
    QPushButton *btnBrowse = new QPushButton("Choose");
    btnBrowse->setObjectName("ChooseButton");
    connect(btnBrowse, &QPushButton::clicked, this, &MainWindow::chooseDir);

    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(lblPath);
    pathLayout->addWidget(lePath);
    pathLayout->addWidget(btnBrowse);
    pathLayout->setSpacing(10);
    pathLayout->setAlignment(Qt::AlignLeft);


    // --- main buttons ---
    QPushButton *btnHelp = new QPushButton("Help");
    btnHelp->setObjectName("HelpButton");
    btnHelp->setFixedSize(90, 35);
    QPushButton *btnCancel = new QPushButton("Cancel");
    btnCancel->setObjectName("CancelButton");
    btnCancel->setFixedSize(90, 35);
    QPushButton *btnDownload = new QPushButton("Download");
    btnDownload->setObjectName("DownloadButton");
    btnDownload->setFixedSize(110, 35);

    connect(btnHelp, &QPushButton::clicked, this, &MainWindow::showHelp);
    connect(btnCancel, &QPushButton::clicked, this, &MainWindow::cancelDownload);
    connect(btnDownload, &QPushButton::clicked, this, &MainWindow::startDownload);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(btnHelp);
    buttonsLayout->addWidget(btnCancel);
    buttonsLayout->addWidget(btnDownload);
    buttonsLayout->setAlignment(Qt::AlignRight);

    QHBoxLayout* path_and_buttons_layout = new QHBoxLayout;
    path_and_buttons_layout->addLayout(pathLayout);
    path_and_buttons_layout->addStretch();
    path_and_buttons_layout->addLayout(buttonsLayout);


    // --- log ---
    log = new QTextEdit;
    log->setReadOnly(true);
    log->setMinimumHeight(200);
    QLabel* console_label = new QLabel("Console output");
    console_label->setObjectName("Label");

    QVBoxLayout* console_output_layout = new QVBoxLayout;
    console_output_layout->addWidget(console_label);
    console_output_layout->addWidget(log);

    // --- main layout ---
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setObjectName("MainLayout");
    mainLayout->addLayout(url_layout);
    mainLayout->addLayout(download_and_custom_name_layout);

    QWidget *videoQualityWidget = new QWidget;
    videoQualityWidget->setLayout(videoQualityLayout);

    QWidget *audioQualityWidget = new QWidget;
    audioQualityWidget->setLayout(audioQualityLayout);

    mainLayout->addWidget(videoQualityWidget);
    mainLayout->addWidget(audioQualityWidget);

    mainLayout->addLayout(path_and_buttons_layout);
    mainLayout->addLayout(console_output_layout);
    // qss
    mainLayout->setSpacing(15);

    QFrame *wrapper = new QFrame;
    wrapper->setObjectName("Wrapper");
    wrapper->setLayout(mainLayout);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(wrapper);
    central->setLayout(centralLayout);


    // quality swap
    audioQualityWidget->setVisible(false);

    cbFormat->clear();
    cbFormat->addItems({"mp4", "mkv"});

    connect(cbMode, &QComboBox::currentTextChanged, this, [=](void) {
        QString mode = cbMode->currentText();
        bool isAudio = (mode == "audio");

        videoQualityWidget->setVisible(!isAudio);
        audioQualityWidget->setVisible(isAudio);

        cbFormat->clear();
        if (mode == "audio")
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

void MainWindow::cancelDownload()
{
    if (!btnIsRunning)
    {
        QMessageBox::information(this, "Cancel", "No download is currently running.");
        return;
    }

    log->append("\nCancelling current download...");

    proc.terminate();

    if (!proc.waitForFinished(3000))
    {
        log->append("Forcing process to stop...");
        proc.kill();
        proc.waitForFinished();
    }

    btnIsRunning = false;

    log->append("Download canceled by user.");
    QMessageBox::information(this, "Canceled", "Download canceled.\nYou may want to delete incomplete files from the destination folder.");
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
