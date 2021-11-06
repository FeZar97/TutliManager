#ifndef WORKER_H
#define WORKER_H

#include <QWidget>
#include <QDebug>
#include <QtWinExtras>
#include <QTime>
#include <QScreen>

#include <windows.h>
#include <map>

#include "src/core/converter.h"

#define SCREEN_INTERVAL                      2000

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

// размеры миникарты в пикселях для разрешения 1920х1080
constexpr static int sizes[]{227, 277, 327, 407, 507, 627};
const static char cTutlsProcessName[]{"WoT Client"};

const static std::map<int, std::map<std::string, QPair<QPointF, QPointF>>> basesRelativeCoords {
    {627, {
            {"Затерянный город", {{0.474, 0.813}, {0.472, 0.131}}}, // +
            {"Карелия",          {{0.083, 0.863}, {0.860, 0.081}}}, // +- (227)
            {"Лайв Окс",         {{0.126, 0.794}, {0.766, 0.120}}}, // +
            {"Ласвилль",         {{0.268, 0.085}, {0.266, 0.860}}}, // +
            {"Малиновка",        {{0.547, 0.855}, {0.110, 0.367}}}, // +
            {"Монастырь",        {{0.493, 0.852}, {0.472, 0.086}}}, // +
            {"Мурованка",        {{0.419, 0.067}, {0.419, 0.869}}}, // +
            {"Перевал",          {{0.810, 0.863}, {0.107, 0.112}}}, // +
            {"Песчаная река",    {{0.836, 0.646}, {0.079, 0.340}}}, // +- (277, 227)
            {"Редшир",           {{0.470, 0.888}, {0.474, 0.073}}}, // +
            {"Степи",            {{0.695, 0.807}, {0.386, 0.120}}}, // +
            {"Тихий берег",      {{0.681, 0.155}, {0.349, 0.769}}}, // +
            {"Фьорды",           {{0.093, 0.349}, {0.861, 0.514}}}, // +
            {"Эрленберг",        {{0.571, 0.874}, {0.378, 0.078}}}, // +
            {"",                 {{0.0, 0.0}, {0.0, 0.0}}},
    }},
    {507, {
            {"Затерянный город", {{0.473, 0.811}, {0.473, 0.136}}},
            {"Карелия",          {{0.085, 0.860}, {0.858, 0.085}}},
            {"Лайв Окс",         {{0.128, 0.791}, {0.763, 0.124}}},
            {"Ласвилль",         {{0.268, 0.087}, {0.268, 0.860}}},
            {"Малиновка",        {{0.546, 0.852}, {0.112, 0.369}}},
            {"Монастырь",        {{0.493, 0.848}, {0.473, 0.091}}},
            {"Мурованка",        {{0.420, 0.069}, {0.420, 0.868}}},
            {"Перевал",          {{0.807, 0.860}, {0.108, 0.114}}},
            {"Редшир",           {{0.469, 0.886}, {0.473, 0.077}}},
            {"Степи",            {{0.694, 0.803}, {0.387, 0.124}}},
            {"Тихий берег",      {{0.679, 0.156}, {0.351, 0.769}}},
            {"Песчаная река",    {{0.081, 0.339}, {0.834, 0.647}}},
            {"Фьорды",           {{0.095, 0.349}, {0.860, 0.515}}},
            {"Эрленберг",        {{0.570, 0.872}, {0.379, 0.083}}},
            {"",                 {{0.0, 0.0}, {0.0, 0.0}}},
    }},
    {407, {
            {"Затерянный город", {{0.474, 0.811}, {0.470, 0.135}}},
            {"Карелия",          {{0.088, 0.857}, {0.850, 0.086}}},
            {"Лайв Окс",         {{0.130, 0.791}, {0.759, 0.125}}},
            {"Ласвилль",         {{0.270, 0.093}, {0.268, 0.853}}},
            {"Малиновка",        {{0.545, 0.850}, {0.113, 0.369}}},
            {"Монастырь",        {{0.491, 0.848}, {0.469, 0.091}}},
            {"Мурованка",        {{0.420, 0.074}, {0.418, 0.862}}},
            {"Перевал",          {{0.803, 0.857}, {0.111, 0.115}}},
            {"Песчаная река",    {{0.086, 0.344}, {0.828, 0.641}}},
            {"Редшир",           {{0.469, 0.885}, {0.469, 0.079}}},
            {"Степи",            {{0.693, 0.803}, {0.386, 0.125}}},
            {"Тихий берег",      {{0.678, 0.160}, {0.349, 0.764}}},
            {"Фьорды",           {{0.098, 0.351}, {0.853, 0.511}}},
            {"Эрленберг",        {{0.570, 0.870}, {0.376, 0.084}}},
            {"",                 {{0.0, 0.0}, {0.0, 0.0}}},
    }},
    {327, {
            {"Затерянный город", {{0.474, 0.804}, {0.474, 0.141}}},
            {"Карелия",          {{0.095, 0.853}, {0.850, 0.092}}},
            {"Лайв Окс",         {{0.138, 0.786}, {0.758, 0.128}}},
            {"Ласвилль",         {{0.275, 0.095}, {0.272, 0.850}}},
            {"Малиновка",        {{0.547, 0.844}, {0.119, 0.370}}},
            {"Монастырь",        {{0.495, 0.841}, {0.474, 0.098}}},
            {"Мурованка",        {{0.422, 0.076}, {0.422, 0.859}}},
            {"Перевал",          {{0.804, 0.853}, {0.116, 0.122}}},
            {"Песчаная река",    {{0.089, 0.346}, {0.829, 0.642}}},
            {"Редшир",           {{0.471, 0.878}, {0.474, 0.083}}},
            {"Степи",            {{0.691, 0.798}, {0.388, 0.131}}},
            {"Тихий берег",      {{0.676, 0.162}, {0.355, 0.761}}},
            {"Фьорды",           {{0.104, 0.352}, {0.853, 0.514}}},
            {"Эрленберг",        {{0.572, 0.862}, {0.379, 0.089}}},
            {"",                 {{0.0, 0.0}, {0.0, 0.0}}},
    }},
    {277, {
            {"Затерянный город", {{0.477, 0.801}, {0.473, 0.148}}},
            {"Карелия",          {{0.097, 0.848}, {0.848, 0.097}}},
            {"Лайв Окс",         {{0.141, 0.783}, {0.758, 0.137}}},
            {"Ласвилль",         {{0.278, 0.101}, {0.274, 0.852}}},
            {"Малиновка",        {{0.545, 0.841}, {0.126, 0.375}}},
            {"Монастырь",        {{0.495, 0.838}, {0.473, 0.105}}},
            {"Мурованка",        {{0.422, 0.083}, {0.423, 0.859}}},
            {"Перевал",          {{0.801, 0.848}, {0.123, 0.126}}},
            {"Песчаная река",    {{0.094, 0.347}, {0., 0.}}},
            {"Редшир",           {{0.473, 0.874}, {0.473, 0.090}}},
            {"Степи",            {{0.690, 0.794}, {0.390, 0.137}}},
            {"Тихий берег",      {{0.675, 0.166}, {0.357, 0.762}}},
            {"Фьорды",           {{0.108, 0.354}, {0.848, 0.516}}},
            {"Эрленберг",        {{0.570, 0.859}, {0.383, 0.097}}},
            {"",                 {{0.0, 0.0}, {0.0, 0.0}}},
    }},
    {227, {
            {"Затерянный город", {{0.476, 0.802}, {0.476, 0.154}}},
            {"Карелия",          {{0.0, 0.0}, {0.0, 0.0}}},
            {"Лайв Окс",         {{0.145, 0.780}, {0.753, 0.141}}},
            {"Ласвилль",         {{0.282, 0.106}, {0.278, 0.846}}},
            {"Малиновка",        {{0.546, 0.837}, {0.132, 0.374}}},
            {"Монастырь",        {{0.493, 0.837}, {0.476, 0.110}}},
            {"Мурованка",        {{0.423, 0.088}, {0.423, 0.855}}},
            {"Перевал",          {{0.797, 0.846}, {0.128, 0.132}}},
            {"Песчаная река",    {{0., 0.}, {0., 0.}}},
            {"Редшир",           {{0.471, 0.872}, {0.476, 0.097}}},
            {"Степи",            {{0.687, 0.793}, {0.392, 0.141}}},
            {"Тихий берег",      {{0.674, 0.172}, {0.357, 0.758}}},
            {"Фьорды",           {{0.115, 0.357}, {0.846, 0.515}}},
            {"Эрленберг",        {{0.568, 0.859}, {0.383, 0.101}}},
            {"",                 {{0.0, 0.0}, {0.0, 0.0}}},
    }}
};

class WORKER : public QObject{
    Q_OBJECT

public:
    HWND                    tutliClient{nullptr};
    LPDWORD                 tutliProcessId;
    DWORD                   tutliThreadId;
    WORD                    xCur;
    WORD                    yCur;

    QTimer                  screenTimer;

    QRect                   screenGeometry;

    int                     tanksNumber;         // количество танков, на которых будет играть кликер
    int                     currentTank{0};      // номер текущего танка
    QVector<QPair<int,int>> centersTanksRegions; // коориданты центров регионов танков

    int                     xBattleButton;
    int                     yBattleButton;

    QString                 curBaseFileName;

    cv::Mat                 lastScreen; // последний скрин окна тутлов
    cv::Mat                 mapAreaImage; // максимальная область с картой (627х627)
    cv::Mat                 currentMapMat; // текущий формат карты
    int                     currentMapSizeIdx; // индекс размера карты в массиве sizes
    bool                    isCurrentMapSizeDetect; // флаг для проверки корректности найденной области с картой
    cv::Mat                 basesMatchMapMat; // карта с найденными базами

    bool                    basesDetectSuccess; // если корелляция по обеим базам > 0.85
    cv::Point               enemyBasePoint;
    cv::Point               unionBasePoint;

    std::vector<cv::Mat>    mapHeaderTemplates;
    std::vector<cv::Mat>    enemyBaseTemplates;
    std::vector<cv::Mat>    unionBaseTemplates;

    WORKER();

    void                    showTutliClient();
    void                    setInitialPosition(int x, int y);
    void                    moveMouseToCoords(int x, int y);
    void                    leftClick();
    void                    calcCentersOfRegions();
    void                    calcStartBattleButton();
    void                    getRandPosInRect(int *x, int *y, int rectX, int rectY, int rectW, int rectH);
    void                    getScreenRegion(int x0, int y0, int w, int h, QImage &img);

    // обработка скрина
    void                    process();
    bool                    makeScreenshot(); // скрин+бан
    void                    detectCurrentMap();
    void                    detectMapNameAndBaseLocation();

    BITMAPINFOHEADER        createBitmapHeader(int width, int height);
    cv::Mat                 captureScreenMat(HWND hwnd);

    void                    startBattle(); // васьки в бой
};

#endif // WORKER_H
