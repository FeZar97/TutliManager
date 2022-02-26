#ifndef MAPSIZEDETECTOR_H
#define MAPSIZEDETECTOR_H

#include <opencv2/imgproc.hpp>

/*
 MapSizeDetector
 Класс для определния размеров карты
*/

enum MapSizeIdx {
    SIZE_UNKNOWN = -1,
    SIZE_1,
    SIZE_2,
    SIZE_3,
    SIZE_4,
    SIZE_5,
    SIZE_6,
};

// относительно размеров 1920x1280
// TODO перевести в относительный размеры и проверить выполнимость
const static std::vector<int> cMapSizes {227, 277, 327, 407, 507, 627};
const static size_t cMapSizesNb{cMapSizes.size()};
const static int cMaxMapSize{cMapSizes.back()};

class MapSizeDetector
{
    // набор шаблонов для опредления конкретного размера карты
    std::vector<cv::Mat> mapHeaderTemplates_;

    // матрица для сохранения результата матчинга
    cv::Mat currentMapMat_;

    // область от карты, приходящей во входных параметрах,
    // в которой может располагаться шапка карты с ДАННЫМ РАЗМЕРОМ
    cv::Mat currentMapPotentionalHeader_;

private:
    // загрузка шаблонных картинок
    void loadMapHeadersTemplates();

public:
    MapSizeDetector() { loadMapHeadersTemplates(); };

    // реальный размер -> MapSizeIdx
    static MapSizeIdx mapSizeToIdx(const int mapSize);

    // MapSizeIdx -> реальный размер
    static int mapIdxToSize(const MapSizeIdx mapSizeIdx);

    // проверка корректности MapSizeIdx
    static bool isMapIdxCorrect(const MapSizeIdx mapSizeIdx);

    // поиск шаблона шапки карты в lastScreen и возвращение найденного размера
    void detectMapSize(const cv::Mat & lastScreen,
                       MapSizeIdx & mapSizeIdx, cv::Mat & currentMapMat);
};

#endif // MAPSIZEDETECTOR_H
