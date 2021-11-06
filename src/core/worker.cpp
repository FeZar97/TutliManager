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

    // read and save templates
    for(size_t i = 0; i < sizeof(sizes) / sizeof(int); i++)
    {
        cv::Mat mapHeaderTempl = cv::imread((QString("C:/FeZar97/TutliManager/src/res/mapsParts/%1.bmp").arg(sizes[i])).toStdString());
        mapHeaderTemplates.push_back(mapHeaderTempl);

        cv::Mat enemyBaseTempl = cv::imread((QString("C:/FeZar97/TutliManager/src/res/red_spawn/%1.bmp").arg(sizes[i])).toStdString());
        enemyBaseTemplates.push_back(enemyBaseTempl);

        cv::Mat unionBaseTempl = cv::imread((QString("C:/FeZar97/TutliManager/src/res/green_spawn/%1.bmp").arg(sizes[i])).toStdString());
        unionBaseTemplates.push_back(unionBaseTempl);
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

// скрин активного танка
void WORKER::screenCurrentTank()
{
    // курсор в центр
    moveMouseToCoords(screenGeometry.width() / 2,  screenGeometry.height() / 2);

    getScreenRegion(TANK_SELECT_REGION_BEGIN_X + TANK_SELECT_BETWEEN_REGION_WIDTH * (currentTank - 1) +  TANK_SELECT_REGION_WIDTH * (currentTank - 1),
                    TANK_SELECT_REGION_BEGIN_Y,
                    TANK_SELECT_REGION_WIDTH,
                    TANK_SELECT_REGION_HEIGHT,
                    currentTankImage);
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

// перебор выбранного кол-ва танков в ангаре
void WORKER::enumerateTanks(int _tanksNumber)
{
    if(tutliClient == nullptr || tanksNumber < 1) return;

    currentTank = _tanksNumber;
    tanksNumber = _tanksNumber;

    calcCentersOfRegions();

    showTutliClient();

    for(auto tankRegion: centersTanksRegions)
    {
        moveMouseToCoords(tankRegion.first, tankRegion.second);
        leftClick();
    }

    screenCurrentTank();
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

    lastScreen = captureScreenMat(tutliClient);
    return true;
}

void WORKER::detectCurrentMap()
{
    int match_method = cv::TM_CCORR_NORMED;

    // get max map area
    int maxMapSize = sizes[mapHeaderTemplates.size() - 1];
    mapAreaImage = lastScreen({lastScreen.size().width - maxMapSize,
                               lastScreen.size().height - maxMapSize,
                               maxMapSize,
                               maxMapSize});
    imwrite("localTemp.bmp", mapAreaImage);
    mapAreaImage = cv::imread("localTemp.bmp");

    double bestMatchVal = 0.;
    int bestMapSizeIdx = -1;
    cv::Point mapCorner;
    canSaveCurrentMap = false;
    for(size_t mapFindIter = 0; mapFindIter < mapHeaderTemplates.size(); mapFindIter++)
    {
        currentMapMat = cv::Mat(cv::Size(mapAreaImage.size().width - mapHeaderTemplates[mapFindIter].size().width + 1,
                                         mapAreaImage.size().height - mapHeaderTemplates[mapFindIter].size().height + 1),
                                      CV_32FC1);

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
            bestMapSizeIdx = mapFindIter;
            mapCorner = maxLoc;
        }
    }

    if(bestMapSizeIdx != -1)
    {
        // qDebug() << QString("Best match for size %1 [%2x%2] result maxVal: %3").arg(bestMapSizeIdx).arg(sizes[bestMapSizeIdx]).arg(bestMatchVal);
        canSaveCurrentMap = true;
        currentMapMat = lastScreen({lastScreen.size().width - sizes[bestMapSizeIdx],
                                          lastScreen.size().height - sizes[bestMapSizeIdx],
                                          sizes[bestMapSizeIdx],
                                          sizes[bestMapSizeIdx]});
        // draw rect with current map
        // rectangle(currentMapMat,
        //           mapCorner,
        //           cv::Point(mapCorner.x + sizes[bestMapSizeIdx], mapCorner.y + sizes[bestMapSizeIdx]),
        //           cv::Scalar(255, 0, 0),
        //           3);
    }
}

void WORKER::detectMapNameAndBaseLocation()
{

}

void WORKER::saveMapArea()
{
    // create dir if need
    if(!QDir(TUTLBOT_HOME_DIR).exists())
    {
        QDir().mkdir(TUTLBOT_HOME_DIR);
    }
    QString file_name = QString("%1map_%2_%3.bmp")
            .arg(TUTLBOT_HOME_DIR)
            .arg(currentMapMat.size().width)
            .arg(QTime::currentTime().toString("hh_mm_ss"));

    // save max map area
    if(canSaveCurrentMap)
    {
        imwrite(file_name.toStdString(), currentMapMat);
    }
}

void WORKER::process()
{
    if(makeScreenshot())
    {
        detectCurrentMap();
        saveMapArea();
    }
}
