#ifndef WORKER_H
#define WORKER_H

#include <QWidget>
#include <QDebug>
#include <QtWinExtras>
#include <QTime>
#include <QScreen>
#include <QDateTime>

#include <windows.h>
#include <map>

#include "converter.h"
#include "tutlscontroller.h"
#include "mapsizedetector.h"
#include "basedetector.h"
#include "mapnamedetector.h"

#define TUTLBOT_HOME_DIR "C:/TUTL/"

class WORKER : public QObject{
    Q_OBJECT

public:
    TutlsController         tutlsController_;
    MapSizeDetector         mapSizeDetector_;
    BaseDetector            baseDetector_;
    MapNameDetector         mapNameDetector_;

    cv::Mat                 lastScreen_; // последний скрин окна тутлов
    cv::Mat                 mapAreaMat_; // максимальная область с картой (627х627)

    // для mapSizeDetector_
    cv::Mat                 currentMapMat_; // текущая карта
    MapSizeIdx              currentMapSizeIdx_;

    // для baseDetector_
    MapName                 currentMapName_{UNKNOWN_MAP_NAME};
    BaseDetectionResult     basesDetectionResult_;
    QLineF                  basesLine_; // линия, соединяющая точки баз

    // куда то деть
    QString                 curBaseFileName;

    // нужно для собирания относительных координат баз
    std::map<int, std::pair<QPointF, QPointF>> curMapRelativeCoords;
    bool zoomInIterations{true};

    // other
    bool                    enableWork{false};
    qint64                  lastProcessTimestamp;
    WORKER();

    // обработка скрина
    void                    process();
    bool                    makeScreenshot(); // скрин+бан

    BITMAPINFOHEADER        createBitmapHeader(int width, int height);
    cv::Mat                 captureScreenMat(HWND hwnd);

public slots:
    void                    startProcess();
};

#endif // WORKER_H
