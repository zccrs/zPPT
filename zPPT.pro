TEMPLATE = app

QT += qml quick multimedia concurrent
CONFIG += c++11

SOURCES += main.cpp \
    utility.cpp \
    speechrecognition.cpp

RESOURCES += qml.qrc \
    images.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

include(xfyun/xfyun.pri)

HEADERS += \
    utility.h \
    speechrecognition.h
