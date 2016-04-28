#include "utility.h"

#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QFileInfo>
#include <QTime>
#include <QDebug>

Utility::Utility(QObject *parent) : QObject(parent)
{

}

QUrl Utility::screenShots() const
{
    const QPixmap &pixmap = qApp->primaryScreen()->grabWindow(0);

    pixmap.save("/tmp/desktop.png", "PNG");

    return QUrl::fromLocalFile("/tmp/desktop.png");
}

bool Utility::fileExists(const QUrl &fileUrl) const
{
    return QFileInfo::exists(fileUrl.toLocalFile());
}

QUrl Utility::toAbsoluteFileUrl(const QString &filePath) const
{
    QFileInfo info(filePath);

    return QUrl::fromLocalFile(info.absoluteFilePath());
}

int Utility::random(int min, int max) const
{
    const QTime &time = QTime::currentTime();
    qsrand(time.msec() + time.second() * 1000);

    return (qrand() % (max - min + 1)) + min;
}

QString Utility::readFile(const QUrl &fileUrl) const
{
    QFile file(fileUrl.toLocalFile());

    if (file.open(QIODevice::ReadOnly)) {
        return QString::fromUtf8(file.readAll());
    }

    return QString();
}
