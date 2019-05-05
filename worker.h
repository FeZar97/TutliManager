#ifndef WORKER_H
#define WORKER_H

#include <QWidget>
#include <QDebug>
#include <QtWinExtras>
#include <QTime>
#include <windows.h>

#include "converter.h"

#define SCREEN_INTERVAL                      5000

#define MOUSE_SINGLE_STEP_DX                 5
#define MOUSE_SINGLE_STEP_DY                 5

#define START_BATTLE_BUTTON_WIDTH            152
#define START_BATTLE_BUTTON_HEIGHT           44
#define START_BATTLE_BUTTON_LEFT_POS         2804
#define START_BATTLE_BUTTON_TOP_POS          4
#define START_BATTLE_BUTTON_R_CENTER_X       0.039583
#define START_BATTLE_BUTTON_R_CENTER_Y       0.024074

#define TANK_SELECT_REGION_WIDTH             160
#define TANK_SELECT_REGION_HEIGHT            98
#define TANK_SELECT_REGION_BEGIN_X           136
#define TANK_SELECT_REGION_BEGIN_Y           935
#define TANK_SELECT_BETWEEN_REGION_WIDTH     16

#define START_BUTTON_WIDTH                   147
#define START_BUTTON_HEIGHT                  40
#define START_BUTTON_X                       886
#define START_BUTTON_Y                       5

#define TUTLBOT_HOME_DIR                     "C:/TUTL/"

// размеры миникарты в пикселях для различных масштабов (разрешение 1920х1080)
constexpr static int sizes[] = {228, 278, 328, 406, 506, 628};

enum PLACE{ VONGARE,
             NAVOINE};

class WORKER : public QObject{
    Q_OBJECT

public:
    HWND                    tutliClient;
    LPDWORD                 tutliProcessId;
    DWORD                   tutliThreadId;
    WORD                    xCur;
    WORD                    yCur;

    PLACE                   currentPlace;

    QTimer                  screenTimer;
    int                     mapSizeIdx;

    int                     screenWidth;
    int                     screenHeight;

    int                     tanksNumber;       // количество танков, на которых будет играть кликер
    int                     currentTank;         // номер текущего танка
    QVector<QPair<int,int>> centersTanksRegions; // коориданты центров регионов танков

    int                     xBattleButton;
    int                     yBattleButton;

    QImage                  lastScreen;
    QImage                  currentTankImage;

    WORKER();

    void                    showTutliClient();
    void                    setInitialPosition(int x, int y);
    void                    moveMouseToCoords(int x, int y);
    void                    leftClick();
    void                    calcCentersOfRegions();
    void                    calcStartBattleButton();
    void                    getRandPosInRect(int *x, int *y, int rectX, int rectY, int rectW, int rectH);
    void                    getScreenRegion(int x0, int y0, int w, int h, QImage &img);

    // обработка изображений
    void                    processImage();

public slots:
    void                    enumerateTanks(int _tanksNumber); // перебор выбранного кол-ва танков в ангаре
    void                    startBattle(); // васьки в бой
    void                    makeScreenshot(); // скрин+бан
    void                    screenCurrentTank(); // скрин текущего активного танка

signals:
    void                    showScreen();
    void                    showCurrentTank();
};

#endif // WORKER_H
