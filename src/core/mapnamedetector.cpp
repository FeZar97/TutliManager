#include "durationlogger.h"
#include "mapsizedetector.h"
#include "basedetector.h"
#include "mapnamedetector.h"

std::string MapNameDetector::mapNameToStr(const MapName mapName)
{
    switch (mapName)
    {
    case Airfild:
        return "Аэродром";
    case Berlin:
        return "Берлин";
    case Wesfield:
        return "Вестфилд";
    case EmpireBorder:
        return "Граница империи";
    case PearlRiver:
        return "Жемчужная река";
    case LostCity:
        return "Затерянный город";
    case Karelia:
        return "Карелия";
    case LiveOaks:
        return "Лайв Окс";
    case Lasville:
        return "Ласвилль";
    case SiegfriedLine:
        return "Линия Зигфрида";
    case MannerheimLine:
        return "Линия Маннергейма";
    case Malinovka:
        return "Малиновка";
    case Minsk:
        return "Минск";
    case Monastery:
        return "Монастырь";
    case Murovanka:
        return "Мурованка";
    case Overlord:
        return "Оверлорд";
    case Paris:
        return "Париж";
    case Pass:
        return "Перевал";
    case SandyRiver:
        return "Песчаная река";
    case Province:
        return "Провинция";
    case IndustrialZone:
        return "Промзона";
    case Prokhorovka:
        return "Прохоровка";
    case Redshire:
        return "Редшир";
    case Mines:
        return "Рудники";
    case Ruinberg:
        return "Руинберг";
    case FishingBay:
        return "Рыбацкая бухта";
    case OldHarbor:
        return "Старая гавань";
    case Steppes:
        return "Степи";
    case Studzianki:
        return "Студзянки";
    case QuietCoast:
        return "Тихий берег";
    case Tundra:
        return "Тундра";
    case Widpark:
        return "Уайдпарк";
    case Cliff:
        return "Утёс";
    case Fjords:
        return "Фьорды";
    case Highway:
        return "Хайвей";
    case Kharkiv:
        return "Харьков";
    case Himmelsdorf:
        return "Химмельсдорф";
    case Shtil:
        return "Штиль";
    case ElKhalluf:
        return "Эль-Халлуф";
    case Ensk:
        return "Энск";
    case Erlenberg:
        return "Эрленберг";

    case UNKNOWN_MAP_NAME:
    default:
        return "UnknownMapName";
    }
}

MapName MapNameDetector::detectMapName(const QPointF &firstBaseRCoord, const QPointF &secondBaseRCoord,
                                       const MapSizeIdx mapSizeIdx, const BaseDetectionResult baseDetectionResult,
                                       // времянка для собирания баз
                                       std::map<int, std::pair<QPointF, QPointF>> & curMapRelativeCoords)
{
    DurationLogger durationLogger("MapNameDetector::detectMapName");

    int curMapSize = MapSizeDetector::mapIdxToSize(mapSizeIdx);

    // случай когда найдены обе базы
    if (baseDetectionResult == BaseDetectionResult::BothBases)
    {
        MapName bestMapNameMatch = UNKNOWN_MAP_NAME;
        float minRDistance1 = cMaxRBaseDistance,
              minRDistance2 = cMaxRBaseDistance;

        // проход по всем картам и поиск наилучшего матчинга
        for (const MapNameWithRCoords & mapData: cBasesRelativeCoords)
        {
            const MapName & curMapName = mapData.first;
            const RCoordsMap & rCoordsMap = mapData.second;

            // если для карты еще нет сохраненных координат
            if (rCoordsMap.empty())
            {
                // Logger::log("For map " + mapNameToStr(curMapName) + " not exist relative coords");
                continue;
            }

            // достаем из RCoordsMap пару с нужным размером карты
            if (const auto rCoordsIter = rCoordsMap.find(curMapSize); rCoordsIter != rCoordsMap.end())
            {
                // относительные координаты баз для карты curMapName и размера карты curMapSize
                QLineF curMapBasesRCoords = (*rCoordsIter).second;

                // считаем дистанцию между firstBaseRCoord и шаблонными базами
                float distance11 = QLineF(firstBaseRCoord,  curMapBasesRCoords.p1()).length(),
                      distance12 = QLineF(firstBaseRCoord,  curMapBasesRCoords.p2()).length(),
                // считаем дистанцию между secondBaseRCoord и шаблонными базами
                      distance21 = QLineF(secondBaseRCoord, curMapBasesRCoords.p1()).length(),
                      distance22 = QLineF(secondBaseRCoord, curMapBasesRCoords.p2()).length();

                // здесь надо рассмотреть два исхода:
                // 1) отображение firstBaseRCoord  -> curMapBasesRCoords.p1()
                //                secondBaseRCoord -> curMapBasesRCoords.p2()
                // error1 = sqrt(distance11^2 + distance22^2)

                //
                // 2) отображение firstBaseRCoord  -> curMapBasesRCoords.p2()
                //                secondBaseRCoord -> curMapBasesRCoords.p1()
                // error2 = sqrt(distance12^2 + distance21^2)

                // решение принимается по минимальной ошибке (максимальное правдоподобие)
                float error1 = sqrtf(distance11 * distance11 + distance22 * distance22),
                      error2 = sqrtf(distance12 * distance12 + distance21 * distance21);

                if (error1 < error2 && error1 <= cTwoBaseRThreshold)
                {
                    minRDistance1 = distance11;
                    minRDistance2 = distance22;
                    bestMapNameMatch = curMapName;
                }
                else if (error2 <= error1 && error2 <= cTwoBaseRThreshold)
                {
                    minRDistance1 = distance21;
                    minRDistance2 = distance12;
                    bestMapNameMatch = curMapName;
                }
                // стремный случай, такого не должно быть
                // else
                // {
                //     Logger::log("Unavailable case for map " + mapNameToStr(curMapName)
                //                 + ", size " + std::to_string(curMapSize) + ", input coords: "
                //                 + "p1{" + std::to_string(firstBaseRCoord.x())  + ", " + std::to_string(firstBaseRCoord.y())  + "}, "
                //                 + "p2{" + std::to_string(secondBaseRCoord.x()) + ", " + std::to_string(secondBaseRCoord.y()) + "}");
                // }

                /*
                // "векторы коллинеарны и ~равны"
                if ( (distance11 <= distance21) && (distance22 <= distance12)
                     && (distance11 < minRDistance1) && (distance22 < minRDistance2) )
                {
                    minRDistance1 = distance11;
                    minRDistance2 = distance22;
                    bestMapNameMatch = curMapName;
                }
                // "векторы противонаправлены и ~равны"
                else if ( (distance21 <= distance11) && (distance12 <= distance22)
                          && (distance21 < minRDistance1) && (distance12 < minRDistance2) )
                {
                    minRDistance1 = distance21;
                    minRDistance2 = distance12;
                    bestMapNameMatch = curMapName;
                }
                // стремный случай, такого не должно быть
                else
                {
                    Logger::log("Unavailable case for map " + mapNameToStr(curMapName)
                                + ", size " + std::to_string(curMapSize) + ", input coords: "
                                + "p1{" + std::to_string(firstBaseRCoord.x())  + ", " + std::to_string(firstBaseRCoord.y())  + "}, "
                                + "p2{" + std::to_string(secondBaseRCoord.x()) + ", " + std::to_string(secondBaseRCoord.y()) + "}");
                }
                */
            }
            // если не существует такой размер надо громко ругнуться
            else
            {
                Logger::log("For size " + std::to_string(curMapSize) + " not exist relative coords (map name: "
                            + mapNameToStr(curMapName) + ")");
            }
        }

        // ВРЕМЯНКА нужно для собирания относительных координат баз
        if(curMapRelativeCoords.find(curMapSize) == curMapRelativeCoords.end())
        {
            Logger::log("Save relative coords for size " + std::to_string(curMapSize));
            curMapRelativeCoords[curMapSize] = {firstBaseRCoord, secondBaseRCoord};

            // как только собираем координаты для всех размеров - выводим результат
            if(curMapRelativeCoords.size() == cMapSizesNb)
            {
                std::string result = "Result ready:\n";
                for(const auto & sizePair: curMapRelativeCoords)
                {
                    result += "\tSize " + std::to_string(sizePair.first) + ": {{" +
                              std::to_string(sizePair.second.first.x())  + ", " + std::to_string(sizePair.second.first.y()) + "}, {" +
                              std::to_string(sizePair.second.second.x()) + ", " + std::to_string(sizePair.second.second.y())
                              + "}},\n";
                }

                Logger::log(result);
            }
        }

        // оба расстояние должны пройти по порогу
        return ( (minRDistance1 > cBaseRThreshold) && (minRDistance2 > cBaseRThreshold) )
                ? UNKNOWN_MAP_NAME
                : bestMapNameMatch;
    }
    // найдена одна из баз (secondBaseRCoord при этом не нужна)
    else if (baseDetectionResult == BaseDetectionResult::OneOfBase)
    {
        MapName bestMapNameMatch = UNKNOWN_MAP_NAME;
        float minRDistance = cMaxRBaseDistance;

        // проход по всем картам и поиск ближайшей базы к координате firstBaseRCoord
        for (const MapNameWithRCoords & mapData: cBasesRelativeCoords)
        {
            const MapName & curMapName = mapData.first;
            const RCoordsMap & rCoordsMap = mapData.second;

            // если для карты еще нет сохраненных координат
            if (rCoordsMap.empty())
            {
                // Logger::log("For map " + mapNameToStr(curMapName) + " not exist relative coords");
                continue;
            }

            // достаем из RCoordsMap пару с нужным размером карты
            if (const auto rCoordsIter = rCoordsMap.find(curMapSize); rCoordsIter != rCoordsMap.end())
            {
                // относительные координаты баз для карты curMapName и размера карты curMapSize
                QLineF curMapBasesRCoords = (*rCoordsIter).second;

                // считаем дистанцию между firstBaseRCoord и шаблонными базами
                float distance1 = QLineF(firstBaseRCoord, curMapBasesRCoords.p1()).length(),
                      distance2 = QLineF(firstBaseRCoord, curMapBasesRCoords.p2()).length();

                // текущая карта подходит лучше
                if (distance1 < minRDistance || distance2 < minRDistance)
                {
                    minRDistance = std::min(distance1, distance2);
                    bestMapNameMatch = curMapName;
                }
            }
            // если не существует такой размер надо громко ругнуться
            else
            {
                Logger::log("For size " + std::to_string(curMapSize) + " not exist relative coords (map name: "
                            + mapNameToStr(curMapName) + ")");
            }
        }

        return minRDistance > cBaseRThreshold ? UNKNOWN_MAP_NAME : bestMapNameMatch;
    }

    // невозможно определить название карты
    return UNKNOWN_MAP_NAME;
}
