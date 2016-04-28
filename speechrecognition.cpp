#include "speechrecognition.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <QUrl>
#include <QtConcurrent/QtConcurrentRun>
#include <QAudioRecorder>
#include <QTemporaryFile>
#include <QDebug>

#define BUFFER_SIZE 4096 * 4
#define FRAME_LEN 640
#define HINTS_SIZE 100

#define AUDIO_FILE "/tmp/zppt.wav"

SpeechRecognition::SpeechRecognition(QObject *parent) : QObject(parent)
{
    int            ret                        =    MSP_SUCCESS;
    const char* login_params            =    "appid = 5720401f, work_dir = ."; // 登录参数，appid与msc库绑定,请勿随意改动

    /* 用户登录 */
    ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数
    if (MSP_SUCCESS != ret)
    {
        qDebug("MSPLogin failed , Error code %d.\n",ret);
//        goto exit; //登录失败，退出登录
    }

    audioRecorder = new QAudioRecorder(this);

    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/PCM");
    audioSettings.setChannelCount(1);
    audioSettings.setQuality(QMultimedia::NormalQuality);
    audioSettings.setSampleRate(16000);

    audioRecorder->setEncodingSettings(audioSettings, QVideoEncoderSettings(), "wav");
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(AUDIO_FILE));
}

void SpeechRecognition::start()
{
    if(audioRecorder->state() != QAudioRecorder::StoppedState)
        return;

    audioRecorder->record();
    tempFile.setFileName(AUDIO_FILE);

    QtConcurrent::run(QThreadPool::globalInstance(), this, &SpeechRecognition::startEngine);
}

void SpeechRecognition::stop()
{
    tempFile.close();
    audioRecorder->stop();
}

bool SpeechRecognition::isRecording() const
{
    return audioRecorder->state() == QAudioRecorder::RecordingState;
}

void SpeechRecognition::recognition(QIODevice *device, const char *session_begin_params)
{
    const char*     session_id                      =    NULL;
    char            hints[HINTS_SIZE]               =    {NULL}; //hints为结束本次会话的原因描述，由用户自定义
    unsigned int    total_len                       =    0;
    int             aud_stat                        =    MSP_AUDIO_SAMPLE_CONTINUE ;        //音频状态
    int             ep_stat                         =    MSP_EP_LOOKING_FOR_SPEECH;        //端点检测
    int             rec_stat                        =    MSP_REC_STATUS_SUCCESS ;            //识别状态
    int             errcode                         =    MSP_SUCCESS ;

    qDebug("\n开始语音听写 ...\n");
    session_id = QISRSessionBegin(NULL, session_begin_params, &errcode); //听写不需要语法，第一个参数为NULL

    qDebug("session id: %s, error code: %d\n", session_id, errcode);

    if (MSP_SUCCESS != errcode)
    {
        qDebug("\nQISRSessionBegin failed! error code:%d\n", errcode);
        goto iat_exit;
    }

    while(true)
    {
        if(!device->isOpen())
            goto iat_exit;

        unsigned int len = 10 * FRAME_LEN; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
        int ret = 0;

        aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;

        if (0 == device->pos())
            aud_stat = MSP_AUDIO_SAMPLE_FIRST;

        char p_pcm[len + 1];

        device->read(p_pcm, len);

        if (len <= 0) {
            QThread::currentThread()->msleep(100);
            continue;
        }

        ret = QISRAudioWrite(session_id, (const void *)p_pcm, len, aud_stat, &ep_stat, &rec_stat);

        if (MSP_SUCCESS != ret)
        {
            qDebug("\nQISRAudioWrite failed! error code:%d\n", ret);
            goto iat_exit;
        }

        if (MSP_REC_STATUS_SUCCESS == rec_stat) //已经有部分听写结果
        {
            const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);

            if (MSP_SUCCESS != errcode)
            {
                qDebug("\nQISRGetResult failed! error code: %d\n", errcode);
                goto iat_exit;
            }
            if (NULL != rslt)
            {
                unsigned int rslt_len = strlen(rslt);
                total_len += rslt_len;
                if (total_len >= BUFFER_SIZE)
                {
                    qDebug("\nno enough buffer for rec_result !\n");
                    goto iat_exit;
                }

//                qDebug("request: %s\n", rslt);

                emit textAppend(QString(rslt));
            }
        }

        if (MSP_EP_AFTER_SPEECH == ep_stat) {
            qDebug() << "ep_stat" << ep_stat;

            break;
        }

        QThread::currentThread()->msleep(210); //模拟人说话时间间隙。200ms对应10帧的音频
    }

//    errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);

//    if (MSP_SUCCESS != errcode)
//    {
//        qDebug("\nQISRAudioWrite failed! error code:%d \n", errcode);
//        goto iat_exit;
//    }

//    while (MSP_REC_STATUS_COMPLETE != rec_stat)
//    {
//        const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
//        if (MSP_SUCCESS != errcode)
//        {
//            qDebug("\nQISRGetResult failed, error code: %d\n", errcode);
//            goto iat_exit;
//        }
//        if (NULL != rslt)
//        {
//            unsigned int rslt_len = strlen(rslt);
//            total_len += rslt_len;
//            if (total_len >= BUFFER_SIZE)
//            {
//                qDebug("\nno enough buffer for rec_result !\n");
//                goto iat_exit;
//            }
//            strncat(rec_result, rslt, rslt_len);
//        }

//        QThread::currentThread()->msleep(150); //防止频繁占用CPU
//    }

iat_exit:
    QISRSessionEnd(session_id, hints);
}

void SpeechRecognition::startEngine()
{
    /*
    * sub:                  请求业务类型
    * domain:               领域
    * language:             语言
    * accent:               方言
    * sample_rate:          音频采样率
    * result_type:          识别结果格式
    * result_encoding:      结果编码格式
    *
    * 详细参数说明请参阅《iFlytek MSC Reference Manual》
    */
    const char* session_begin_params    =    "sub = iat, domain = iat, language = zh_ch, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = utf8";

    if(tempFile.open(QIODevice::ReadOnly)) {
        while(true) {
            recognition(&tempFile, session_begin_params);

            if(audioRecorder->state() != QAudioRecorder::RecordingState)
                break;

            tempFile.readAll();

            QThread::msleep(500);
        }

        tempFile.close();

        qDebug() << "finished!!!!";
    }
}
