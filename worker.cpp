#include "worker.h"

WORKER::WORKER()
{
    screenWidth = 1920;
    screenHeight = 1080;

    currentTank = 1;

    xCur = 960;
    yCur = 540;

    mapSizeIdx = 1;

    // поиск тутлов
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString s = codec->toUnicode("WoT Client");
    LPCWSTR lps = LPCWSTR(s.utf16());

    HWND hWnd = FindWindow(nullptr, lps);
    if(hWnd != nullptr) tutliClient = hWnd;

    qsrand(uint(QTime::currentTime().msec()));

    screenTimer.setInterval(SCREEN_INTERVAL);
    connect(&screenTimer, &QTimer::timeout, this, &WORKER::makeScreenshot);
    screenTimer.start();

    lastScreen = QImage(":/DED.jpg");
    emit showScreen();
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

// скрин активного танка
void WORKER::screenCurrentTank()
{
    // курсор в центр
    moveMouseToCoords(screenWidth/2, screenHeight/2);

    getScreenRegion(TANK_SELECT_REGION_BEGIN_X + TANK_SELECT_BETWEEN_REGION_WIDTH * (currentTank - 1) +  TANK_SELECT_REGION_WIDTH * (currentTank - 1),
                    TANK_SELECT_REGION_BEGIN_Y,
                    TANK_SELECT_REGION_WIDTH,
                    TANK_SELECT_REGION_HEIGHT,
                    currentTankImage);

    emit showCurrentTank();
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

    for(auto tankRegion: centersTanksRegions){
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
void WORKER::makeScreenshot()
{
    //создание скрина области карты
    QScreen *screen = QGuiApplication::primaryScreen();
    int width = screen->size().width(),
        height = screen->size().height();
    lastScreen = screen->grabWindow(0, 0, 0, width, height).toImage();
    emit showScreen();

    //QPixmap originalPixmap = screen->grabWindow(0, width - sizes[mapSizeIdx], height - sizes[mapSizeIdx], sizes[mapSizeIdx], sizes[mapSizeIdx]);
    //QString file_name = QString(TUTLBOT_HOME_DIR) + QTime::currentTime().toString("hh_mm_ss") + ".bmp";
    //originalPixmap.save(file_name, "BMP", 100);
}
