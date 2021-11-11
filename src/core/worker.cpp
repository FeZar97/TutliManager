#include <opencv2/imgcodecs.hpp>
// #include <WinUser.h>
#include "logger.h"
#include "worker.h"

WORKER::WORKER()
{
    const QScreen *curScreen = QGuiApplication::primaryScreen();
    screenGeometry = curScreen->geometry();

    // set cursor to screen center
    xCur = screenGeometry.width() / 2;
    yCur = screenGeometry.height() / 2;

    tryFindTutliProcess();

    mapHeaderTemplates.resize(cMapSizesNb);
    enemyBaseTemplates.resize(cMapSizesNb);
    unionBaseTemplates.resize(cMapSizesNb);

    // read and save templates
    for(size_t i = 0; i < cMapSizesNb; i++)
    {
        cv::imread((QString("C:/FeZar97/TutliManager/src/res/mapsParts/%1.bmp").arg(cMapSizes[i])).toStdString()).convertTo(mapHeaderTemplates[i], CV_32FC4);
        cv::cvtColor(mapHeaderTemplates[i], mapHeaderTemplates[i], cv::COLOR_BGRA2BGR);

        cv::imread((QString("C:/FeZar97/TutliManager/src/res/red_spawn/%1.bmp").arg(cMapSizes[i])).toStdString()).convertTo(enemyBaseTemplates[i], CV_32FC4);
        cv::cvtColor(enemyBaseTemplates[i], enemyBaseTemplates[i], cv::COLOR_BGRA2BGR);

        cv::imread((QString("C:/FeZar97/TutliManager/src/res/green_spawn/%1.bmp").arg(cMapSizes[i])).toStdString()).convertTo(unionBaseTemplates[i], CV_32FC4);
        cv::cvtColor(unionBaseTemplates[i], unionBaseTemplates[i], cv::COLOR_BGRA2BGR);
    }

    // screenTimer.setInterval(SCREEN_INTERVAL);
    // connect(&screenTimer, &QTimer::timeout, this, &WORKER::process);
    // screenTimer.start();
}

// расчет координат для выбранного количества танков
void WORKER::calcCentersOfRegions()
{
    centersTanksRegions.clear();

    for(int i = 0; i < tanksNumber; i++){
        int xCenterReg = TANK_SELECT_REGION_BEGIN_X + // начало полоски выбора танков по X
                         TANK_SELECT_BETWEEN_REGION_WIDTH * i + // смещение относительно уже пройденных регионов
                         TANK_SELECT_REGION_WIDTH * i + // расстояние между регионами
                         TANK_SELECT_REGION_WIDTH/2 +
                         qrand()%(TANK_SELECT_REGION_WIDTH - TANK_SELECT_REGION_WIDTH/6) - (TANK_SELECT_REGION_WIDTH - TANK_SELECT_REGION_WIDTH/6)/2, // рандомная координата внутри ширины региона

            yCenterReg = TANK_SELECT_REGION_BEGIN_Y +// начало полоски выбора танков по Y
                         TANK_SELECT_REGION_HEIGHT/2 +
                         qrand()%(TANK_SELECT_REGION_HEIGHT - TANK_SELECT_REGION_HEIGHT/6) - (TANK_SELECT_REGION_HEIGHT - TANK_SELECT_REGION_HEIGHT/6)/2;

        centersTanksRegions.push_back(QPair<int, int>(xCenterReg, yCenterReg));
    }
}

// расчет координат кнопки начала
void WORKER::calcStartBattleButton()
{
    getRandPosInRect(&xBattleButton, &yBattleButton, START_BUTTON_X, START_BUTTON_Y, START_BUTTON_WIDTH, START_BUTTON_HEIGHT);
}

// рандомная точка внутри прямоугольника
void WORKER::getRandPosInRect(int *x, int *y, int rectX, int rectY, int rectW, int rectH)
{
    // рандомная точка внутри прямоугольника
    *x = rectX + qrand()%rectW;
    *y = rectY + qrand()%rectH;
}

// скриншот выбранной области и сохранение в img
void WORKER::getScreenRegion(int x0, int y0, int w, int h, QImage &img)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    img = screen->grabWindow(0, x0, y0, w, h).toImage();
}

// поиск процесса тутлов
void WORKER::tryFindTutliProcess()
{
    if(tutliClient)
    {
        return;
    }

    if(HWND hWnd = FindWindow(nullptr, cTutlsProcessName);
            hWnd != nullptr)
    {
        tutliClient = hWnd;
        tutlsController.setTutlsProcess(hWnd);
    }
    else
    {
        log("Not found tutliClient");
        tutliClient = nullptr;
    }
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

// помещение курсора в центр и инициализация координат
void WORKER::setInitialPosition(int x, int y)
{
    // showTutliClient();

    xCur = x;
    yCur = y;

    SetCursorPos(xCur, yCur);
}

// перемещение мышки на нужны координатыъ
void WORKER::moveMouseToCoords(int x, int y)
{
    while(xCur != x || yCur != y){

        int dx = (abs(xCur - x) < MOUSE_SINGLE_STEP_DX) ?
                    ((xCur - x) * -1) : (MOUSE_SINGLE_STEP_DX * (xCur - x < 0 ? 1 : -1));
        xCur += dx;

        int dy = (abs(yCur - y) < MOUSE_SINGLE_STEP_DX) ?
                    ((yCur - y) * -1) : (MOUSE_SINGLE_STEP_DY * (yCur - y < 0 ? 1 : -1));
        yCur += dy;

        SetCursorPos(xCur, yCur);
        mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
        Sleep(12);
    }
}

// нажатие ЛКМ со случайным временем между press&release
void WORKER::leftClick()
{
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    Sleep(90 + (15 - qrand()%30));
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    Sleep(800 + (200 - qrand()%400));
}

// в бой
void WORKER::startBattle()
{
    if(tutliClient == nullptr) return;
    calcStartBattleButton();
    // showTutliClient();
    moveMouseToCoords(xBattleButton, yBattleButton);
    leftClick();
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
    if(!tutliClient)
    {
        return false;
    }

    qint64 beginTimestamp = QDateTime::currentMSecsSinceEpoch();

    // create dir if need
    if(!QDir(TUTLBOT_HOME_DIR).exists())
    {
        QDir().mkdir(TUTLBOT_HOME_DIR);
    }

    // create unique file name
    curBaseFileName = QString("%1%2")
                        .arg(TUTLBOT_HOME_DIR)
                        .arg(QTime::currentTime().toString("hh_mm_ss"));

    captureScreenMat(tutliClient).convertTo(lastScreen, CV_32FC4);
    cv::cvtColor(lastScreen, lastScreen, cv::COLOR_BGRA2BGR);

    // save origin
    // imwrite(curBaseFileName.toStdString() + "_origin.bmp", lastScreen);

    qint64 endTimestamp = QDateTime::currentMSecsSinceEpoch();
    log("[time] Duration of 'makeScreenshot': " + std::to_string(endTimestamp - beginTimestamp) + "ms");
    return true;
}

// определение текущего размера карты и сохранение области карты в currentMapMat
void WORKER::detectCurrentMap()
{
    qint64 beginTimestamp = QDateTime::currentMSecsSinceEpoch();

    // get max map area
    int maxMapSize = cMapSizes.back();
    mapAreaImage = lastScreen({lastScreen.size().width - maxMapSize,
                               lastScreen.size().height - maxMapSize,
                               maxMapSize,
                               maxMapSize});

    currentMapSizeIdx = -1;
    double bestMatchVal = 0., minVal = 0., maxVal = 0.;
    cv::Point mapCorner, minLoc, maxLoc;
    size_t curTemplateIdx = 0;

    // проход по всем шаблонам шапок мини карт и сверка их с текущей картой
    for(auto mapHeaderTemplate: mapHeaderTemplates)
    {
        // выделение памяти под результат матчинга+
        currentMapMat = cv::Mat(cv::Size(maxMapSize - mapHeaderTemplate.size().width + 1,
                                         maxMapSize - mapHeaderTemplate.size().height + 1),
                                CV_32FC3);

        // ищем нужный шаблон
        matchTemplate(mapAreaImage, mapHeaderTemplate, currentMapMat, cv::TM_CCORR_NORMED);
        minMaxLoc(currentMapMat, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

        // log("For map size " + std::to_string(mapHeaderTemplate.size().width) + " match value: " + std::to_string(maxVal));

        // для корректно найденного угла карты выполняется условие: (maxLoc.x == maxLoc.y)
        if(maxVal > bestMatchVal && (maxLoc.x == maxLoc.y) && maxVal > 0.95)
        {
            bestMatchVal = maxVal;
            currentMapSizeIdx = curTemplateIdx;
        }
        else
        {
            curTemplateIdx++;
        }
    }

    // на данном этапе должен быть известен currentMapSizeIdx
    isCurrentMapSizeDetect = (currentMapSizeIdx >= 0 && currentMapSizeIdx < mapHeaderTemplates.size());
    if(isCurrentMapSizeDetect)
    {
        // сохраняем размеры карт и объектов
        currentMapSize = cMapSizes[currentMapSizeIdx];
        currentBaseSize = cBaseSizes[currentMapSizeIdx];

        // log("Best match for size " + std::to_string(currentMapSize) + " with match result " + std::to_string(bestMatchVal));
        log("Current map size: " + std::to_string(currentMapSize));
        // log("Current base size: " + std::to_string(currentBaseSize));

        // сохранение текущей карты в Mat
        currentMapMat = lastScreen({lastScreen.size().width - currentMapSize,
                                    lastScreen.size().height - currentMapSize,
                                    currentMapSize,
                                    currentMapSize});

        // save current map area
        // imwrite(QString("%1_map_%2.bmp").arg(curBaseFileName).arg(currentMapSize).toStdString(), currentMapMat);
    }
    else
    {
        log("Cant detect map idx");
    }

    qint64 endTimestamp = QDateTime::currentMSecsSinceEpoch();
    log("Duration of 'detectCurrentMap': " + std::to_string(endTimestamp - beginTimestamp) + "ms");
}

// для найденного масштаба карты ищем базы и по относительным координатам баз определяем название карты
void WORKER::detectMapNameAndBaseLocation()
{
    if(!isCurrentMapSizeDetect)
    {
        return;
    }

    qint64 beginTimestamp = QDateTime::currentMSecsSinceEpoch();

    basesDetectSuccess = false;

    // инициализация для сохранения результата
    basesMatchMapMat = cv::Mat(cv::Size(currentMapSize - currentBaseSize + 1, currentMapSize - currentBaseSize + 1),
                               CV_32FC3);

    double minVal, enemyMaxVal, unionMaxVal;
    cv::Point minLoc, enemyMaxLoc, unionMaxLoc;

// match enemy base
    matchTemplate(currentMapMat, enemyBaseTemplates[currentMapSizeIdx], basesMatchMapMat, cv::TM_CCORR_NORMED);
    minMaxLoc(basesMatchMapMat, &minVal, &enemyMaxVal, &minLoc, &enemyMaxLoc, cv::Mat());
    // qDebug() << QString("Best match of enemy base: (%1, %2) with val %3").arg(enemyMaxLoc.x).arg(enemyMaxLoc.y).arg(enemyMaxVal);

// match union base
    matchTemplate(currentMapMat, unionBaseTemplates[currentMapSizeIdx], basesMatchMapMat, cv::TM_CCORR_NORMED);
    minMaxLoc(basesMatchMapMat, &minVal, &unionMaxVal, &minLoc, &unionMaxLoc, cv::Mat());
    // qDebug() << QString("Best match of union base: (%1, %2) with val %3").arg(unionMaxLoc.x).arg(unionMaxLoc.y).arg(unionMaxVal);

    // вычисление относительный координат найденных баз
    // делаем это для инвариантности расположения базы от размера карты
    int halfBaseSize = currentBaseSize / 2;
    QPointF enemyBaseCenterRCoords{(enemyMaxLoc.x + halfBaseSize) / float(currentMapSize),
                                   (enemyMaxLoc.y + halfBaseSize) / float(currentMapSize)},
            unionBaseCenterRCoords{(unionMaxLoc.x + halfBaseSize) / float(currentMapSize),
                                   (unionMaxLoc.y + halfBaseSize) / float(currentMapSize)};

    // вычисляем относительную дистанцию между базами для принятия решения о корректности найденных баз
    float relativeDistanceBetweenBases = QLineF(enemyBaseCenterRCoords, unionBaseCenterRCoords).length();

    if(enemyMaxVal < 0.85)
    {
        log("Can't find ENEMY base, max correlation val: " + std::to_string(enemyMaxVal));
    }
    else
    {
        // поиск в словаре для текущего размера карты "похожих" координат
        const std::map<std::string, QPair<QPointF, QPointF>> &curSizeMaps = basesRelativeCoords.at(cMapSizes[currentMapSizeIdx]);
        for(auto mapPair: curSizeMaps)
        {
            const QPair<QPointF, QPointF>& baseRPoints = mapPair.second;
            const QPointF &base1RPoint = baseRPoints.first,
                          &base2RPoint = baseRPoints.second;

            if(QLineF(enemyBaseCenterRCoords, base1RPoint).length() < 0.01 ||
               QLineF(enemyBaseCenterRCoords, base2RPoint).length() < 0.01)
            {
                log("Map name (by enemy base): " + mapPair.first);
            }
        }
    }

    if(unionMaxVal < 0.85)
    {
        log("Can't find UNION base, max correlation val: " + std::to_string(unionMaxVal));
    }
    else
    {
        // поиск в словаре для текущего размера карты "похожих" координат
        const std::map<std::string, QPair<QPointF, QPointF>> &curSizeMaps = basesRelativeCoords.at(cMapSizes[currentMapSizeIdx]);
        for(auto mapPair: curSizeMaps)
        {
            const QPair<QPointF, QPointF>& baseRPoints = mapPair.second;
            const QPointF &base1RPoint = baseRPoints.first,
                          &base2RPoint = baseRPoints.second;

            if(QLineF(unionBaseCenterRCoords, base1RPoint).length() < 0.01 ||
               QLineF(unionBaseCenterRCoords, base2RPoint).length() < 0.01)
            {
                log("Map name (by union base): " + mapPair.first);
            }
        }
    }

    if(relativeDistanceBetweenBases <= 0.25)
    {
        log("Too small distance between bases: " + std::to_string(relativeDistanceBetweenBases));
    }
    else
    {
        // log("Cur bases relative coords: {{" +
        //         std::to_string(enemyBaseCenterRCoords.x()) + ", " + std::to_string(enemyBaseCenterRCoords.y()) + "}, {" +
        //         std::to_string(unionBaseCenterRCoords.x()) + ", " + std::to_string(unionBaseCenterRCoords.y())
        //         + "}},"
        //     );

        if(curMapRelativeCoords.find(currentMapSize) == curMapRelativeCoords.end())
        {
            log("Save relative coords for size " + std::to_string(currentMapSize));
            curMapRelativeCoords[currentMapSize] = {enemyBaseCenterRCoords, unionBaseCenterRCoords};

            if(curMapRelativeCoords.size() == cMapSizesNb)
            {
                std::string result = "Result ready:\n";
                for(auto sizePair: curMapRelativeCoords)
                {
                    result += "\tSize " + std::to_string(sizePair.first) + ": {{" +
                              std::to_string(sizePair.second.first.x())  + ", " + std::to_string(sizePair.second.first.y()) + "}, {" +
                              std::to_string(sizePair.second.second.x()) + ", " + std::to_string(sizePair.second.second.y())
                              + "}},\n";
                }

                QFile resultFile(curBaseFileName + ".txt");
                if(resultFile.open(QIODevice::WriteOnly))
                {
                    resultFile.write(QByteArray::fromStdString(result));
                    resultFile.close();
                }

                log(result);
                curMapRelativeCoords.clear();
            }
        }
        else
        {
            // log("Relative coords for size " + std::to_string(currentMapSize) + " already exists");
        }

        // qDebug() << QString("Bases relative coords: {{%1, %2}, {%3, %4}}},")
        //             .arg(QString::number(enemyBaseRCoords.x(), 'f', 3)).arg(QString::number(enemyBaseRCoords.y(), 'f', 3))
        //             .arg(QString::number(unionBaseRCoords.x(), 'f', 3)).arg(QString::number(unionBaseRCoords.y(), 'f', 3));
    }

    basesDetectSuccess = (enemyMaxVal >= 0.85) && (unionMaxVal >= 0.85) && (relativeDistanceBetweenBases > 0.4);

// draw bases rects
    if(basesDetectSuccess)
    {
        log("Both bases are successfull detected");

        // пытаемся зумить карту пока есть неизвестные координаты
        if(curMapRelativeCoords.size() != cMapSizesNb)
        {
            // define orderMapScaleIteration
            if(currentMapSize == cMapSizes.back())
            {
                zoomInIterations = false;
            }
            if(currentMapSize == cMapSizes.front())
            {
                zoomInIterations = true;
            }

            // apply order
            if(zoomInIterations)
            {
                tutlsController.zoomInMinimap();
            }
            else
            {
                tutlsController.zoomOutMinimap();
            }
        }

        // rectangle(currentMapMat, enemyMaxLoc,
        //           cv::Point(enemyMaxLoc.x + enemyBaseTemplates[currentMapSizeIdx].size().width,
        //                     enemyMaxLoc.y + enemyBaseTemplates[currentMapSizeIdx].size().height),
        //           cv::Scalar(0, 0, 255), 1);

        // rectangle(currentMapMat, unionMaxLoc,
        //           cv::Point(unionMaxLoc.x + enemyBaseTemplates[currentMapSizeIdx].size().width,
        //                     unionMaxLoc.y + enemyBaseTemplates[currentMapSizeIdx].size().height),
        //           cv::Scalar(0, 255, 0), 1);

        // // save map area with bases detection
        // imwrite(QString("%1_bases.bmp").arg(curBaseFileName).toStdString(), currentMapMat);
    }

    qint64 endTimestamp = QDateTime::currentMSecsSinceEpoch();
    log("Duration of 'detectMapNameAndBaseLocation': " + std::to_string(endTimestamp - beginTimestamp) + "ms");
}

void WORKER::process()
{
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    qint64 dt = timestamp - lastProcessTimestamp;

    if(dt < 150)
    {
        QThread::msleep(dt);
        return;
    }

    log("New prcoess cycle");
    lastProcessTimestamp = timestamp;
    tryFindTutliProcess();

    if(makeScreenshot())
    {
        detectCurrentMap();
        detectMapNameAndBaseLocation();
    }
}
