#ifndef SPEECHRECOGNITION_H
#define SPEECHRECOGNITION_H

#include <QObject>
#include <QTemporaryFile>

QT_BEGIN_NAMESPACE
class QAudioRecorder;
QT_END_NAMESPACE

class SpeechRecognition : public QObject
{
    Q_OBJECT
public:
    explicit SpeechRecognition(QObject *parent = 0);

public slots:
    void start();
    void stop();
    bool isRecording() const;

signals:
    void textAppend(const QString &text);

private:
    QAudioRecorder *audioRecorder = Q_NULLPTR;
    QFile tempFile;

    void recognition(QIODevice *device, const char* session_begin_params);
    void startEngine();
};

#endif // SPEECHRECOGNITION_H
