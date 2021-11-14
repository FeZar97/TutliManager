#ifndef TUTLSCONTROLLER_H
#define TUTLSCONTROLLER_H

#include <windows.h>
#include <WinUser.h>

class QString;

enum class MovingDirection
{
    Forward,
    Back,
    Left,
    Right
};

const static LPCWSTR cTutlsProcessName{L"WoT Client"};

class TutlsController
{
    HWND tutlsHandler_{nullptr};

    void SendKyboardEvent(const int vkKey, const bool down) const;

    QString directionToStr(const MovingDirection direction) const;
    int directionToVkKey(const MovingDirection direction) const;

public:
    TutlsController() {};

    bool showTutliWindow() const;

    bool tryFindTutliProcess();
    HWND getTutliProcess() const;

    // mini map
    void zoomInMinimap() const;
    void zoomOutMinimap() const;

    // mooving  actions
    void startMoving(const MovingDirection direction) const;
    void stopMoving(const MovingDirection direction) const;
};

#endif // TUTLSCONTROLLER_H
