#include <opencv2/imgcodecs.hpp>

#include "worker.h"

WORKER::WORKER()
{
    const QScreen *curScreen = QGuiApplication::primaryScreen();
    screenGeometry = curScreen->geometry();

    // set cursor to screen center
    xCur = screenGeometry.width() / 2;
    yCur = screenGeometry.height() / 2;

    // поиск процесса тутлов
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString s = codec->toUnicode(cTutlsProcessName);
    LPCWSTR lps = LPCWSTR(s.utf16());

    HWND hWnd = FindWindow(nullptr, lps);
    if(hWnd != nullptr)
    {
        tutliClient = hWnd;
    }

    mapHeaderTemplates.resize(sizeof(sizes) / sizeof(int));
    enemyBaseTemplates.resize(sizeof(sizes) / sizeof(int));
    unionBaseTemplates.resize(sizeof(sizes) / sizeof(int));

    // read and save templates
    for(size_t i = 0; i < sizeof(sizes) / sizeof(int); i++)
    {
        cv::imread((QString("C:/FeZar97/TutliManager/src/res/mapsParts/%1.bmp").arg(sizes[i])).toStdString()).convertTo(mapHeaderTemplates[i], CV_32FC4);
        cv::cvtColor(mapHeaderTemplates[i], mapHeaderTemplates[i], cv::COLOR_BGRA2BGR);
        // qDebug() << "type of mapHeaderTemplates[idx]: " << QString::fromStdString(static_cast<std::string>(cv::typeToString(mapHeaderTemplates[i].type())));

        cv::imread((QString("C:/FeZar97/TutliManager/src/res/red_spawn/%1.bmp").arg(sizes[i])).toStdString()).convertTo(enemyBaseTemplates[i], CV_32FC4);
        cv::cvtColor(enemyBaseTemplates[i], enemyBaseTemplates[i], cv::COLOR_BGRA2BGR);

        cv::imread((QString("C:/FeZar97/TutliManager/src/res/green_spawn/%1.bmp").arg(sizes[i])).toStdString()).convertTo(unionBaseTemplates[i], CV_32FC4);
        cv::cvtColor(unionBaseTemplates[i], unionBaseTemplates[i], cv::COLOR_BGRA2BGR);
    }

    screenTimer.setInterval(SCREEN_INTERVAL);
    connect(&screenTimer, &QTimer::timeout, this, &WORKER::process);
    screenTimer.start();
}

// вывод окна танков на передний план
void WORKER::showTutliClient()
{
    if(tutliClient != nullptr){
        ShowWindow(tutliClient, SW_RESTORE);
        SetForegroundWindow(tutliClient);
        SetCursorPos(xCur, yCur);
        UpdateWindow(tutliClient);
    }
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
    showTutliClient();

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
    showTutliClient();
    moveMouseToCoords(xBattleButton, yBattleButton);
    leftClick();
}

// скрин+бан
bool WORKER::makeScreenshot()
{
    if(!tutliClient)
    {
        qDebug() << "Not found tutliClient";
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

    captureScreenMat(tutliClient).convertTo(lastScreen, CV_32FC4);
    cv::cvtColor(lastScreen, lastScreen, cv::COLOR_BGRA2BGR);

    // save origin
    // imwrite(curBaseFileName.toStdString() + "_origin.bmp", lastScreen);

    return true;
}

// определение текущего размера карты и сохранение области карты в currentMapMat
void WORKER::detectCurrentMap()
{
    int match_method = cv::TM_CCORR_NORMED;

    // get max map area
    int maxMapSize = sizes[mapHeaderTemplates.size() - 1];
    mapAreaImage = lastScreen({lastScreen.size().width - maxMapSize,
                               lastScreen.size().height - maxMapSize,
                               maxMapSize,
                               maxMapSize});

    double bestMatchVal = 0.;
    cv::Point mapCorner;
    currentMapSizeIdx = -1;
    isCurrentMapSizeDetect = false;
    for(size_t mapFindIter = 0; mapFindIter < mapHeaderTemplates.size(); mapFindIter++)
    {
        currentMapMat = cv::Mat(cv::Size(mapAreaImage.size().width - mapHeaderTemplates[mapFindIter].size().width + 1,
                                         mapAreaImage.size().height - mapHeaderTemplates[mapFindIter].size().height + 1),
                                CV_32FC3);

        // qDebug() << "type of mapAreaImage: " << QString::fromStdString(static_cast<std::string>(cv::typeToString(mapAreaImage.type())));
        // qDebug() << "type of mapHeaderTemplates[idx]: " << QString::fromStdString(static_cast<std::string>(cv::typeToString(mapHeaderTemplates[mapFindIter].type())));
        // qDebug() << "type of currentMapMat: " << QString::fromStdString(static_cast<std::string>(cv::typeToString(currentMapMat.type())));

        // ищем нужный шаблон
        matchTemplate(mapAreaImage, mapHeaderTemplates[mapFindIter], currentMapMat, match_method);
        // normalize(findMapCornerResult, findMapCornerResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

        double minVal, maxVal;
        cv::Point minLoc, maxLoc;

        minMaxLoc(currentMapMat, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

        // qDebug() << QString("iter %1 d1, d2: (%2, %3)").arg(mapFindIter).arg(lastScreen.size().width - matchLoc.x).arg(lastScreen.size().height - matchLoc.y);
        // qDebug() << QString("iter %1 [map size %2] result minVal: %3").arg(mapFindIter).arg(sizes[mapFindIter]).arg(minVal);
        // qDebug() << QString("iter %1 [map size %2] result maxVal: %3").arg(mapFindIter).arg(sizes[mapFindIter]).arg(maxVal);

        if(maxVal > bestMatchVal && maxLoc.x == maxLoc.y)
        {
            bestMatchVal = maxVal;
            currentMapSizeIdx = mapFindIter;
            mapCorner = maxLoc;
        }
    }

    if(currentMapSizeIdx >= 0 && currentMapSizeIdx < mapHeaderTemplates.size())
    {
        // qDebug() << QString("Best match for size %1 [%2x%2] result maxVal: %3").arg(bestMapSizeIdx).arg(sizes[bestMapSizeIdx]).arg(bestMatchVal);
        qDebug() << QString("Current map size: %1").arg(sizes[currentMapSizeIdx]);
        isCurrentMapSizeDetect = true;
        currentMapMat = lastScreen({lastScreen.size().width - sizes[currentMapSizeIdx],
                                          lastScreen.size().height - sizes[currentMapSizeIdx],
                                          sizes[currentMapSizeIdx],
                                          sizes[currentMapSizeIdx]});
        // draw rect with current map
        // rectangle(currentMapMat,
        //           mapCorner,
        //           cv::Point(mapCorner.x + sizes[bestMapSizeIdx], mapCorner.y + sizes[bestMapSizeIdx]),
        //           cv::Scalar(255, 0, 0),
        //           3);

        // save current map area
        // if(isCurrentMapSizeDetect)
        // {
        //     imwrite(QString("%1_map_%2.bmp").arg(curBaseFileName).arg(sizes[currentMapSizeIdx]).toStdString(), currentMapMat);
        // }
    }
    else
    {
        qDebug() << "invalid map idx: " << currentMapSizeIdx;
    }
}

// для найденного масштаба карты ищем вражескую базу
//
void WORKER::detectMapNameAndBaseLocation()
{
    if(!isCurrentMapSizeDetect)
    {
        // qDebug() << "can't detect map name and base location";
        return;
    }

    basesDetectSuccess = false;

    basesMatchMapMat = cv::Mat(cv::Size(currentMapMat.size().width  - enemyBaseTemplates[currentMapSizeIdx].size().width  + 1,
                                        currentMapMat.size().height - enemyBaseTemplates[currentMapSizeIdx].size().height + 1),
                               CV_32FC3);

    // qDebug() << "type of currentMapMat: " << QString::fromStdString(static_cast<std::string>(cv::typeToString(currentMapMat.type())));
    // qDebug() << "type of enemyBaseTemplate: " << QString::fromStdString(static_cast<std::string>(cv::typeToString(enemyBaseTemplates[currentMapSizeIdx].type())));
    // qDebug() << "type of basesMatchMapMat: " << QString::fromStdString(static_cast<std::string>(cv::typeToString(basesMatchMapMat.type())));

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

    float distanceBetweenBases = QLineF({float(enemyMaxLoc.x), float(enemyMaxLoc.y)},
                                        {float(unionMaxLoc.x), float(unionMaxLoc.y)}).length();

    QPointF enemyBaseRCoords{enemyMaxLoc.x / float(currentMapMat.size().width), enemyMaxLoc.y / float(currentMapMat.size().height)},
            unionBaseRCoords{unionMaxLoc.x / float(currentMapMat.size().width), unionMaxLoc.y / float(currentMapMat.size().height)};

    if(enemyMaxVal < 0.85)
    {
        qDebug() << QString("Can't find enemy base (%1)").arg(enemyMaxVal);
    }
    else
    {
        // define map name with ENEMY base rCoords
        const std::map<std::string, QPair<QPointF, QPointF>> &curSizeMaps = basesRelativeCoords.at(sizes[currentMapSizeIdx]);
        for(auto mapPair: curSizeMaps)
        {
            const QPair<QPointF, QPointF>& baseRPoints = mapPair.second;
            const QPointF &base1RPoint = baseRPoints.first,
                          &base2RPoint = baseRPoints.second;

            if(QLineF(enemyBaseRCoords, base1RPoint).length() < 0.01 ||
               QLineF(enemyBaseRCoords, base2RPoint).length() < 0.01)
            {
                qDebug() << QString("Map name (by enemy base): %1").arg(QString::fromStdString(mapPair.first));
            }
        }
    }

    if(unionMaxVal < 0.85)
    {
        qDebug() << QString("Can't find union base (%1)").arg(unionMaxVal);
    }
    else
    {
        // define map name with UNION base rCoords
        const std::map<std::string, QPair<QPointF, QPointF>> &curSizeMaps = basesRelativeCoords.at(sizes[currentMapSizeIdx]);
        for(auto mapPair: curSizeMaps)
        {
            const QPair<QPointF, QPointF>& baseRPoints = mapPair.second;
            const QPointF &base1RPoint = baseRPoints.first,
                          &base2RPoint = baseRPoints.second;

            if(QLineF(unionBaseRCoords, base1RPoint).length() < 0.01 ||
               QLineF(unionBaseRCoords, base2RPoint).length() < 0.01)
            {
                qDebug() << QString("Map name (by union base): %1").arg(QString::fromStdString(mapPair.first));
            }
        }
    }

    if(distanceBetweenBases < 0.4 * float(currentMapMat.size().width))
    {
        qDebug() << QString("Too small distance between bases (%1)").arg(distanceBetweenBases);
    }
    else
    {
        qDebug() << QString("Bases relative coords: {{%1, %2}, {%3, %4}}},")
                    .arg(QString::number(enemyBaseRCoords.x(), 'f', 3)).arg(QString::number(enemyBaseRCoords.y(), 'f', 3))
                    .arg(QString::number(unionBaseRCoords.x(), 'f', 3)).arg(QString::number(unionBaseRCoords.y(), 'f', 3));

        // qDebug() << QString("Enemy relative base coord: (%1, %2)").arg(enemyBaseRCoords.x()).arg(enemyBaseRCoords.y());
        // qDebug() << QString("Union relative base coord: (%1, %2)").arg(unionBaseRCoords.x()).arg(unionBaseRCoords.y());
    }

    basesDetectSuccess = (enemyMaxVal >= 0.85) && (unionMaxVal >= 0.85) && (distanceBetweenBases > 0.4 * currentMapMat.size().width);

// draw bases rects
    if(basesDetectSuccess)
    {
        rectangle(currentMapMat, enemyMaxLoc,
                  cv::Point(enemyMaxLoc.x + enemyBaseTemplates[currentMapSizeIdx].size().width,
                            enemyMaxLoc.y + enemyBaseTemplates[currentMapSizeIdx].size().height),
                  cv::Scalar(0, 0, 255), 1);

        rectangle(currentMapMat, unionMaxLoc,
                  cv::Point(unionMaxLoc.x + enemyBaseTemplates[currentMapSizeIdx].size().width,
                            unionMaxLoc.y + enemyBaseTemplates[currentMapSizeIdx].size().height),
                  cv::Scalar(0, 255, 0), 1);

    // save map area with bases detection
        imwrite(QString("%1_bases.bmp").arg(curBaseFileName).toStdString(), currentMapMat);
    }
    else
    {
        qDebug() << "Can't detect bases";
    }
}

void WORKER::process()
{
    if(makeScreenshot())
    {
        detectCurrentMap();
        detectMapNameAndBaseLocation();
    }
}
