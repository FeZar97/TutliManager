#-------------------------------------------------
#
# Project created by QtCreator 2019-03-08T01:44:34
#
#-------------------------------------------------

QT       += core gui
QT       += winextras
QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TutliManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        widget.cpp \
    worker.cpp \
    converter.cpp

HEADERS += \
        widget.h \
    worker.h \
    converter.h

FORMS += \
        widget.ui

LIBS += -luser32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += C:\OpenCV\opencv\build\include

LIBS += C:\OpenCV\opencv-build\bin\libopencv_core344.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_highgui344.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_imgcodecs344.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_imgproc344.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_features2d344.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_calib3d344.dll

DISTFILES +=

RESOURCES += \
    res.qrc
