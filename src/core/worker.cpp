#include <opencv2/imgcodecs.hpp>

#include "logger.h"
#include "durationlogger.h"
#include "mapsizedetector.h"

#include "worker.h"

WORKER::WORKER() {
    drawAndSaveBases();
}

// https://superkogito.github.io/blog/CaptureScreenUsingOpenCv.html
BITMAPINFOHEADER WORKER::createBitmapHeader(int width, int height)
{
    BITMAPINFOHEADER  bi;

    // create a bitmap
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    return bi;
}

// https://superkogito.github.io/blog/CaptureScreenUsingOpenCv.html
cv::Mat WORKER::captureScreenMat(HWND hwnd)
{
    cv::Mat src;

    // get handles to a device context (DC)
    HDC hwindowDC = GetDC(hwnd);
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    // define scale, height and width
    int screenx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screeny = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // create mat object
    src.create(height, width, CV_8UC4);

    // create a bitmap
    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    BITMAPINFOHEADER bi = createBitmapHeader(width, height);

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);

    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);            //copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}

void WORKER::drawAndSaveBases()
{
    int sideSize = 1000;
    cv::Mat resultMat = cv::Mat(sideSize, sideSize, CV_8UC3);

    // проход по всем картам
    for (const MapNameWithRCoords & mapData: cBasesRelativeCoords)
    {
        const RCoordsMap & rCoordsMap = mapData.second;

        // если для карты еще нет сохраненных координат
        if (rCoordsMap.empty())
        {
            continue;
        }
        const std::string mapName = MapNameDetector::mapNameToStr(mapData.first);
        const QLineF & maxPrecisionCoords = rCoordsMap.at(cMapSizes.back());

        int rCh = ((-mapName[0])), //  -  47.) / 32.) * 255.,
            gCh = ((-mapName[1])), //  - 104.) / 32.) * 255.,
            bCh = ((-mapName[2])); //  - 104.) / 32.) * 255.;

        Logger::log("Scalar for " + mapName + ": ("
                    + std::to_string(rCh) + ", "
                    + std::to_string(gCh) + ", "
                    + std::to_string(bCh) + ")");

        cv::line(resultMat,
                 {static_cast<int>(maxPrecisionCoords.x1() * sideSize), static_cast<int>(maxPrecisionCoords.y1() * sideSize)},
                 {static_cast<int>(maxPrecisionCoords.x2() * sideSize), static_cast<int>(maxPrecisionCoords.y2() * sideSize)},
                 cv::Scalar(rCh, gCh, bCh),
                 1,
                 cv::LINE_AA);
    }

    imwrite(std::string(TUTLBOT_HOME_DIR) + "lines.bmp", resultMat);
}

void WORKER::startProcess()
{
    lastProcessTimestamp = QDateTime::currentMSecsSinceEpoch();
    enableWork = true;
    while (enableWork)
    {
        process();
    }
}

// скрин+бан
bool WORKER::makeScreenshot()
{
    DurationLogger durationLogger("WORKER::makeScreenshot");

    HWND tutlsProcessHandler = tutlsController_.getTutliProcess();

    if(tutlsProcessHandler == nullptr)
    {
        return false;
    }

    // create dir if need
    if(!QDir(TUTLBOT_HOME_DIR).exists())
    {
        QDir().mkdir(TUTLBOT_HOME_DIR);
    }

    // create unique file name
    curBaseFileName = QString("%1%2")
                        .arg(TUTLBOT_HOME_DIR)
                        .arg(QTime::currentTime().toString("hh_mm_ss"));

    captureScreenMat(tutlsProcessHandler).convertTo(lastScreen_, CV_32FC4);

    // save origin
    // imwrite(curBaseFileName.toStdString() + "_origin.png", lastScreen);

    return true;
}

void WORKER::process()
{
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    qint64 dt = timestamp - lastProcessTimestamp;

    if(dt < 1500)
    {
        QThread::msleep(dt);
        return;
    }

    Logger::log("-------------------------------------------------");
    lastProcessTimestamp = timestamp;
    tutlsController_.tryFindTutliProcess();

    if(!makeScreenshot())
    {
        return;
    }

    // определение размеров карты
    // передаем всю карту и ищем в ней шаблоны шапок
    mapSizeDetector_.detectMapSize(lastScreen_, currentMapSizeIdx_, currentMapMat_);

    // поиск баз на текущей карте
    baseDetector_.detectBases(currentMapMat_, currentMapSizeIdx_, basesLine_, basesDetectionResult_);

    // определение названия текущей карты
    mapNameDetector_.detectMapName(basesLine_.p1(), basesLine_.p2(), currentMapSizeIdx_, basesDetectionResult_,
                                   currentMapName_,
                                   // времянка для собирания баз
                                   curMapRelativeCoords);

    Logger::log("Map name detection result: " + MapNameDetector::mapNameToStr(currentMapName_));

    // ui
    emit setMapParams(currentMapSizeIdx_, currentMapName_);

    // нужно для собирания относительных координат баз
    if (basesDetectionResult_ == BaseDetectionResult::BothBases
        && currentMapName_ == MapName::UNKNOWN_MAP_NAME)
    {
        Logger::log("Both bases are successfull detected");

        int curMapSize = MapSizeDetector::mapIdxToSize(currentMapSizeIdx_);

        // пытаемся зумить карту пока есть неизвестные координаты
        if (curMapRelativeCoords.size() != cMapSizesNb)
        {
            // определяем направление зума
            if (curMapSize == cMapSizes.back())
            {
                zoomInIterations = false;
            }
            if (curMapSize == cMapSizes.front())
            {
                zoomInIterations = true;
            }

            if (zoomInIterations)
            {
                tutlsController_.zoomInMinimap();
            }
            else
            {
                tutlsController_.zoomOutMinimap();
            }
        }
    }
}
