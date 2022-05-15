#include <opencv2/imgcodecs.hpp>

#include "durationlogger.h"
#include "mapsizedetector.h"

void MapSizeDetector::loadMapHeadersTemplates()
{
    DurationLogger durationLogger("MapSizeDetector::loadMapHeadersTemplates");
    mapHeaderTemplates_.resize(cMapSizesNb);

    for (size_t i = 0; i < cMapSizesNb; i++)
    {
        cv::imread("C:/FeZar97/TutliManager/src/res/mapsParts/" + std::to_string(cMapSizes[i]) + ".png", cv::IMREAD_UNCHANGED)
                .convertTo(mapHeaderTemplates_[i], CV_32FC4);
        // Logger::log("matWithMap mapHeaderTemplates_[i]: " + std::to_string(mapHeaderTemplates_[i].depth()));
        // Logger::log("matWithMap mapHeaderTemplates_[i]: " + cv::typeToString(mapHeaderTemplates_[i].type()));
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
    return isMapIdxCorrect(mapSizeIdx) ? cMapSizes.at(mapSizeIdx) : cMapSizes.at(0);
}

bool MapSizeDetector::isMapIdxCorrect(const MapSizeIdx mapSizeIdx)
{
    return (mapSizeIdx >=0 && static_cast<size_t>(mapSizeIdx) < cMapSizes.size());
}

void MapSizeDetector::detectMapSize(const cv::Mat & lastScreen,
                                    MapSizeIdx & mapSizeIdx, cv::Mat & currentMapMat)
{
    DurationLogger durationLogger("MapSizeDetector::detectMapSize");

    double bestMatchVal = 0., minVal = 0., maxVal = 0.;
    cv::Point mapCorner, minLoc, maxLoc;

    MapSizeIdx bestMapSizeIdx = SIZE_UNKNOWN;

    // проход по всем шаблонам шапок мини карт и их сверка с текущей картой
    int curTemplateIdx = 0;
    for (const auto &mapHeaderTemplate: mapHeaderTemplates_)
    {
        int curPotentialMapSize = mapIdxToSize(static_cast<MapSizeIdx>(curTemplateIdx));

        // DurationLogger durationLogger("MapSizeDetector::detectMapSize::compareWithTemplate_" + std::to_string(curPotentialMapSize));

        // копируем кусок карты с той областью, где может быть шапка ДЛЯ ТЕКУЩЕГО РАЗМЕРА
        int startX = lastScreen.size().width - curPotentialMapSize,
            startY = lastScreen.size().height - curPotentialMapSize;
        currentMapPotentionalHeader_ = lastScreen({startX - 5,
                                                   startY - 5,
                                                   mapHeaderTemplate.size().width + 5,
                                                   mapHeaderTemplate.size().height + 5});

        // выделение памяти под результат матчинга
        // ищем в вырезанном куске currentMapPotentionalHeader_ шаблон для текущего размера
        currentMapMat_ = cv::Mat(cv::Size(currentMapPotentionalHeader_.size().width - mapHeaderTemplate.size().width + 1,
                                          currentMapPotentionalHeader_.size().height - mapHeaderTemplate.size().height + 1),
                                 CV_32FC4);

        // ищем нужный шаблон
        matchTemplate(currentMapPotentionalHeader_, mapHeaderTemplate, currentMapMat_, cv::TM_CCORR_NORMED);
        minMaxLoc(currentMapMat_, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

        // Logger::log("For map size " + std::to_string(mapHeaderTemplates_.size().width) + " match value: " + std::to_string(maxVal));

        // для корректно найденного угла карты выполняется условие: (maxLoc.x == maxLoc.y)
        // эмпирически подобранный порог = 0.95 (фактические значения ~0.99)
        if(maxVal > bestMatchVal && (maxLoc.x == maxLoc.y) && maxVal > 0.95)
        {
            bestMatchVal = maxVal;
            bestMapSizeIdx = static_cast<MapSizeIdx>(curTemplateIdx);
        }
        curTemplateIdx++;
    }

    // сохраняем лучший размер
    mapSizeIdx = bestMapSizeIdx;

    // на данном этапе должен быть известен bestMapSizeIdx
    if (isMapIdxCorrect(bestMapSizeIdx))
    {
        // Logger::log("Best match for size " + std::to_string(currentMapSize) + " with match result " + std::to_string(bestMatchVal));
        Logger::log("Current map size: " + std::to_string(mapIdxToSize(bestMapSizeIdx)));

        // сохранение текущей карты в Mat
        int currentMapSize = mapIdxToSize(bestMapSizeIdx);
        currentMapMat = lastScreen({lastScreen.size().width - currentMapSize,
                                    lastScreen.size().height - currentMapSize,
                                    currentMapSize,
                                    currentMapSize});
    }
    else
    {
        Logger::log("Cant detect map idx (incorrect idx " + std::to_string(bestMapSizeIdx) + ")");
    }
}
