#include "utility.h"

#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QFileInfo>

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
