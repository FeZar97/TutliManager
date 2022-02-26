#ifndef BASEDETECTOR_H
#define BASEDETECTOR_H

#include <QLineF>
#include <opencv2/imgproc.hpp>

// FWD
enum MapSizeIdx;

enum BaseDetectionResult {
    BothBases, // надены обе базы
    OneOfBase, // найдена одна из баз
    NoneBase   // не найдено ни одной базы
};

// относительно размеров 1920x1280
// TODO перевести в относительный размеры и проверить выполнимость
const static std::vector<int> cBaseSizes{24,  27,  30,  34,  40,  47};
const static QPointF cInvalidBaseCoords{-1.f, -1.f};

/*
 BaseDetector
 Класс для определния расположения баз
*/
class BaseDetector
{
    // порог для сравнения шаблона с куском карты
    const float successDetectionThreshold_{0.85f};
    // порог для дистанции между базами
    const float baseDistanceThreshold_{0.25f};

    // карта с найденными базами
    cv::Mat basesMatchMapMat_;

    // потенциальные точки баз (центрированные, нормированные)
    cv::Point enemyBasePoint_;
    cv::Point unionBasePoint_;

    // шаблоны изображений баз для разных размеров
    std::vector<cv::Mat> enemyBaseTemplates_;
    std::vector<cv::Mat> unionBaseTemplates_;

private:
    // загрузка шаблонных картинок
    void loadBasesTemplates();

public:
    BaseDetector() { loadBasesTemplates(); };

    // размер базы -> MapSizeIdx
    static MapSizeIdx baseSizeToMapSizeIdx(const int baseSize);

    // MapSizeIdx -> размер базы
    static int mapIdxToBaseSize(const MapSizeIdx mapSizeIdx);

    static std::string detectRes2Str(const BaseDetectionResult detectRes);

    // метод определния расположения баз
    // в basesLine сохранятется линия с относительными координатами баз
    // если одна из баз не найдена, то в basesLine первой точкой будет найденная база
    //                                      а второй - cInvalidBaseCoords
    void detectBases(const cv::Mat & map, const MapSizeIdx mapSizeIdx,
                     QLineF & basesLine, BaseDetectionResult &detectionResult);
};

#endif // BASEDETECTOR_H
