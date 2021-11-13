#-------------------------------------------------
#
# Project created by QtCreator 2019-03-08T01:44:34
#
#-------------------------------------------------

QT       += core gui winextras core

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

CONFIG += c++17

SOURCES += \
	src/main.cpp \
	src/core/mapsizedetector.cpp \
	src/core/durationlogger.cpp \
	src/core/worker.cpp \
	src/core/converter.cpp \
	src/core/tutlscontroller.cpp \
	src/ui/widget.cpp

HEADERS += \
	src/core/mapsizedetector.h \
	src/core/durationlogger.h \
	src/core/logger.h \
	src/core/worker.h \
	src/core/converter.h \
	src/core/tutlscontroller.h \
	src/ui/widget.h

LIBS += -luser32 \
	-lGdi32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += C:\OpenCV\opencv-4.5.4\build\include

LIBS += C:\OpenCV\opencv-4.5.4\bin\lib\Debug\opencv_core454d.lib \
	C:\OpenCV\opencv-4.5.4\bin\lib\Debug\opencv_imgproc454d.lib \
	C:\OpenCV\opencv-4.5.4\bin\lib\Debug\opencv_imgcodecs454d.lib \
# 	C:\OpenCV\opencv-4.5.4\bin\lib\Debug\opencv_highgui454d.lib \
# 	C:\OpenCV\opencv-4.5.4\bin\lib\Debug\opencv_features2d454d.lib \
# 	C:\OpenCV\opencv-4.5.4\bin\lib\Debug\opencv_calib3d454d.lib

RESOURCES += \
    src/res.qrc
