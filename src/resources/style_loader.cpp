#include "style_loader.hpp"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <QApplication>
#include <QFontDatabase>


static QString readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Could not load stylesheet file:" << path;
        return {};
    }

    QTextStream stream(&file);
    return stream.readAll();
}

QString loadStyleSheet(Theme theme)
{
    QString variablesFile;
    switch (theme)
    {
        case Theme::Light: variablesFile = ":/qss/light_var.qss"; break;
        case Theme::Dark: variablesFile = ":/qss/dark_var.qss"; break;
    }

    QString variables = readFile(variablesFile);
    QString style = readFile(":/qss/style.qss");

    // manually variable togle
    QStringList lines = variables.split('\n');
    QMap<QString, QString> colors;
    for (const QString &line : lines)
    {
        if (line.startsWith("@define-color"))
        {
            QStringList parts = line.split(QRegularExpression("\\s+"));
            if (parts.size() >= 3)
                colors.insert(parts[1], parts[2].trimmed().remove(';'));
        }
    }

    // toggle all style.qss variables
    for (auto it = colors.begin(); it != colors.end(); it++)
        style.replace("@" + it.key(), it.value(), Qt::CaseSensitive);

    if (theme == Theme::Dark) {
        style.replace("@icon_down_arrow", ":/icons/down_arrow_dark.png");
        style.replace("@moon_icon", ":/icons/moon_dark.png");
        style.replace("@sun_icon", ":/icons/sun_dark.png");
    } else {
        style.replace("@icon_down_arrow", ":/icons/down_arrow_light.png");
        style.replace("@moon_icon", ":/icons/moon_light.png");
        style.replace("@sun_icon", ":/icons/sun_light.png");
    }

    return style;
}