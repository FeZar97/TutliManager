#include <opencv2/imgcodecs.hpp>

#include "durationlogger.h"
#include "mapsizedetector.h"
#include "basedetector.h"

void BaseDetector::loadBasesTemplates()
{
    enemyBaseTemplates_.reserve(cMapSizesNb);
    unionBaseTemplates_.reserve(cMapSizesNb);

    for (size_t i = 0; i < cMapSizesNb; i++)
    {
        // TODO убрать абсолютные пути, перейти на svg/png
        cv::imread("C:/FeZar97/TutliManager/src/res/red_spawn/" + std::to_string(cMapSizes[i]) + ".bmp")
                .convertTo(enemyBaseTemplates_[i], CV_32FC4);
        cv::cvtColor(enemyBaseTemplates_[i], enemyBaseTemplates_[i], cv::COLOR_BGRA2BGR);

        // TODO убрать абсолютные пути, перейти на svg/png
        cv::imread("C:/FeZar97/TutliManager/src/res/green_spawn/" + std::to_string(cMapSizes[i]) + ".bmp")
                .convertTo(unionBaseTemplates_[i], CV_32FC4);
        cv::cvtColor(unionBaseTemplates_[i], unionBaseTemplates_[i], cv::COLOR_BGRA2BGR);
    }
}

MapSizeIdx BaseDetector::baseSizeToMapSizeIdx(const int baseSize)
{
    auto findRes = std::find(cBaseSizes.begin(), cBaseSizes.end(), baseSize);
    if (findRes == cBaseSizes.end())
    {
        return MapSizeIdx::SIZE_UNKNOWN;
    }
    else
    {
        return MapSizeIdx(findRes - cBaseSizes.begin());
    }
}

int BaseDetector::mapIdxToBaseSize(const MapSizeIdx mapSizeIdx)
{
    return MapSizeDetector::isMapIdxCorrect(mapSizeIdx) ? cBaseSizes.at(mapSizeIdx) : -1;
}

BaseDetectionResult BaseDetector::detectBases(const cv::Mat & map, const MapSizeIdx mapSizeIdx,
                                             QLineF & basesLine)
{
    DurationLogger durationLogger("BaseDetector::detectBases");

    int curMapSize = MapSizeDetector::mapIdxToSize(mapSizeIdx);
    int curBaseSize = mapIdxToBaseSize(mapSizeIdx);

    if (map.size().width != map.size().height // карта должна быть квадратной
        || !MapSizeDetector::isMapIdxCorrect(mapSizeIdx) // проверка корректности индекса размера карты
        || map.size().width != curMapSize // проверка на совпадение индекса размера и фактического размера
        || curBaseSize == -1) // проверка размера шаблона базы
    {
        log("Invalid input data for method BaseDetector::detectBases");
    }

    const cv::Mat & enemyBaseTemplate = enemyBaseTemplates_.at(mapSizeIdx),
                    unionBaseTemplate = unionBaseTemplates_.at(mapSizeIdx);

    // инициализация для сохранения результата
    basesMatchMapMat_ = cv::Mat(cv::Size(curMapSize - curBaseSize + 1, curMapSize - curBaseSize + 1),
                                CV_32FC3);

    double minVal, enemyMaxVal, unionMaxVal;
    cv::Point minLoc, enemyMaxLoc, unionMaxLoc;

// match enemy base
    matchTemplate(map, enemyBaseTemplate, basesMatchMapMat_, cv::TM_CCORR_NORMED);
    minMaxLoc(basesMatchMapMat_, &minVal, &enemyMaxVal, &minLoc, &enemyMaxLoc, cv::Mat());

// match union base
    matchTemplate(map, unionBaseTemplate, basesMatchMapMat_, cv::TM_CCORR_NORMED);
    minMaxLoc(basesMatchMapMat_, &minVal, &unionMaxVal, &minLoc, &unionMaxLoc, cv::Mat());

    // вычисление относительный координат найденных баз
    // делаем это для инвариантности расположения базы от размера карты
    int halfBaseSize = curBaseSize / 2;
    QPointF enemyBaseCenterRCoords{(enemyMaxLoc.x + halfBaseSize) / float(curMapSize),
                                   (enemyMaxLoc.y + halfBaseSize) / float(curMapSize)},
            unionBaseCenterRCoords{(unionMaxLoc.x + halfBaseSize) / float(curMapSize),
                                   (unionMaxLoc.y + halfBaseSize) / float(curMapSize)};

    // сохраняем координаты в линию
    basesLine = {enemyBaseCenterRCoords, unionBaseCenterRCoords};

    // вычисляем относительную дистанцию между базами для принятия решения о корректности найденных баз
    float relativeDistanceBetweenBases = QLineF(enemyBaseCenterRCoords, unionBaseCenterRCoords).length();

    bool isEnemyBaseDetectSuccess = (enemyMaxVal >= successDetectionThreshold_),
         isUnionBaseDetectSuccess = (unionMaxVal >= successDetectionThreshold_),
         isDistanceThresholdCorrect = (relativeDistanceBetweenBases >= baseDistanceThreshold_);

    // if (!isEnemyBaseDetectSuccess)
    // {
    //     log("Can't find ENEMY base, max correlation val: " + std::to_string(enemyMaxVal));
    // }
    // if (!isUnionBaseDetectSuccess)
    // {
    //     log("Can't find UNION base, max correlation val: " + std::to_string(unionMaxVal));
    // }
    // if (!isDistanceThresholdCorrect)
    // {
    //     log("Too small distance between bases: " + std::to_string(relativeDistanceBetweenBases));
    // }

    // найдены обе базы
    if (isDistanceThresholdCorrect)
    {
        return BothBases;
    }
    // найдена вражеская база
    else if (isEnemyBaseDetectSuccess)
    {
        basesLine = {enemyBaseCenterRCoords, cInvalidBaseCoords};
        return OneOfBase;
    }
    else if (isUnionBaseDetectSuccess)
    {
        basesLine = {unionBaseCenterRCoords, cInvalidBaseCoords};
        return OneOfBase;
    }
    // базы не найдены
    return NoneBase;
}