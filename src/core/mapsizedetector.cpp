#include <opencv2/imgcodecs.hpp>

#include "durationlogger.h"
#include "mapsizedetector.h"

void MapSizeDetector::loadMapHeadersTemplates()
{
    DurationLogger durationLogger("loadMapHeadersTemplates");
    mapHeaderTemplates_.resize(cMapSizesNb);

    for (size_t i = 0; i < cMapSizesNb; i++)
    {
        cv::imread("/res/mapsParts/" + std::to_string(cMapSizes[i])).convertTo(mapHeaderTemplates_[i], CV_32FC4);
        cv::cvtColor(mapHeaderTemplates_[i], mapHeaderTemplates_[i], cv::COLOR_BGRA2BGR);
    }
}

void MapSizeDetector::detectMapSize(const cv::Mat & matWithMap,
                                    MapSizeIdx & mapSizeIdx, cv::Mat & currentMapMat)
{
    DurationLogger durationLogger("detectMapSize");

    double bestMatchVal = 0., minVal = 0., maxVal = 0.;
    cv::Point mapCorner, minLoc, maxLoc;

    MapSizeIdx bestMapSizeIdx = SIZE_UNKNOWN;

    // проход по всем шаблонам шапок мини карт и их сверка с текущей картой
    int curTemplateIdx = 0;
    for (const auto &mapHeaderTemplate: mapHeaderTemplates_)
    {
        // выделение памяти под результат матчинга+
        currentMapMat_ = cv::Mat(cv::Size(cMaxMapSize - mapHeaderTemplate.size().width + 1,
                                          cMaxMapSize - mapHeaderTemplate.size().height + 1),
                                 CV_32FC3); // TODO: избавить от CV_32FC3 (перейти на png файлы)

        // ищем нужный шаблон
        matchTemplate(matWithMap, mapHeaderTemplate, currentMapMat, cv::TM_CCORR_NORMED);
        minMaxLoc(currentMapMat, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

        // log("For map size " + std::to_string(mapHeaderTemplates_.size().width) + " match value: " + std::to_string(maxVal));

        // для корректно найденного угла карты выполняется условие: (maxLoc.x == maxLoc.y)
        // эмпирически подобранный порог = 0.95 (фактические значения ~0.99)
        if(maxVal > bestMatchVal && (maxLoc.x == maxLoc.y) && maxVal > 0.95)
        {
            bestMatchVal = maxVal;
            bestMapSizeIdx = static_cast<MapSizeIdx>(curTemplateIdx);
        }
        curTemplateIdx++;
    }

    mapSizeIdx = bestMapSizeIdx;

    // на данном этапе должен быть известен currentMapSizeIdx
    if (isMapIdxCorrect(bestMapSizeIdx))
    {
        // log("Best match for size " + std::to_string(currentMapSize) + " with match result " + std::to_string(bestMatchVal));
        log("Current map size: " + std::to_string(mapIdxToSize(mapSizeIdx)));

        // сохранение текущей карты в Mat
        int currentMapSize = mapIdxToSize(bestMapSizeIdx);
        currentMapMat = matWithMap({matWithMap.size().width - currentMapSize,
                                    matWithMap.size().height - currentMapSize,
                                    currentMapSize,
                                    currentMapSize});
    }
    else
    {
        log("Cant detect map idx");
    }
}

MapSizeIdx MapSizeDetector::mapSizeToIdx(const int mapSize)
{
    auto findRes = std::find(cMapSizes.begin(), cMapSizes.end(), mapSize);
    if (findRes == cMapSizes.end())
    {
        return MapSizeIdx::SIZE_UNKNOWN;
    }
    else
    {
        return MapSizeIdx(findRes - cMapSizes.begin());
    }
}

int MapSizeDetector::mapIdxToSize(const MapSizeIdx mapSizeIdx)
{
    return isMapIdxCorrect(mapSizeIdx) ? cMapSizes.at(mapSizeIdx) : -1;
}

bool MapSizeDetector::isMapIdxCorrect(const MapSizeIdx mapSizeIdx)
{
    return (mapSizeIdx >=0 && static_cast<size_t>(mapSizeIdx) < cMapSizes.size());
}
