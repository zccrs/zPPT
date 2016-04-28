#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCursor>
#include <QPixmap>
#include <QQuickView>
#include <QTimer>
#include <QDebug>
#include <speechrecognition.h>

#include "utility.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickView view;

    QObject::connect(view.engine(), &QQmlEngine::quit, &app, &QGuiApplication::quit);

    view.rootContext()->setContextProperty("utility", new Utility(&view));
    view.rootContext()->setContextProperty("speechRecognition", new SpeechRecognition(&view));
    view.setSource(QUrl("qrc:///main.qml"));
    view.setFlags(Qt::SplashScreen);
    view.showFullScreen();

    QTimer::singleShot(2000, [&view] {
        QCursor cursor(QPixmap(":/images/avatar.png").scaled(30, 30));

        view.setCursor(cursor);
    });

    return app.exec();
}
