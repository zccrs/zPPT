#ifndef UTILITY_H
#define UTILITY_H

#include <QObject>
#include <QUrl>

class Utility : public QObject
{
    Q_OBJECT
public:
    explicit Utility(QObject *parent = 0);

public slots:
    QUrl screenShots() const;
    bool fileExists(const QUrl &fileUrl) const;
    QUrl toAbsoluteFileUrl(const QString &filePath) const;

    int random(int min, int max) const;

    QString readFile(const QUrl &fileUrl) const;
};

#endif // UTILITY_H
