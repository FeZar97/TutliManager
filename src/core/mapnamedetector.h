#ifndef MAPNAMEDETECTOR_H
#define MAPNAMEDETECTOR_H

#include <map>
#include <string>
#include <QLineF>

#include "MapsData.h"

// FWD
enum MapSizeIdx;
enum BaseDetectionResult;

// максимильная относительная дистанция между базами (если базы находится в противоположных углах квадрата)
const static float cMaxRBaseDistance{sqrtf(2.f)};

// порог относительного отклонения найденной базы от шаблонных относительных координат
const static float cBaseRThreshold{0.01f};

// порог для суммы ошибок по двум базам
const static float cTwoBaseRThreshold{cBaseRThreshold * sqrtf(2.f)};

/*
 MapNameDetector
 Класс для определния названия карты
*/
class MapNameDetector
{
public:
    MapNameDetector() {};

    // перевод MapName -> std::string
    static std::string mapNameToStr(const MapName mapName);

    // метод определния название карты по относительным координатам баз
    // возвращает имя карты в виде элемента перечисления MapName
    static MapName detectMapName(const QPointF & firstBaseRCoord, const QPointF & secondBaseRCoord,
                                 const MapSizeIdx mapSizeIdx, const BaseDetectionResult baseDetectionResult,
                                 // времянка для собирания баз
                                 std::map<int, std::pair<QPointF, QPointF>> & curMapRelativeCoords);
};

#endif // MAPNAMEDETECTOR_H
