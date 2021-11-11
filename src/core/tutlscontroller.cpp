#include <QString>

#include "logger.h"
#include "tutlscontroller.h"

void TutlsController::SendKyboardEvent(const int vkKey, const bool down) const
{
    // https://forum.vingrad.ru/forum/topic-338301.html
    UINT scan = MapVirtualKey(vkKey, MAPVK_VK_TO_VSC);
    INPUT inp = {0};
    inp.type = INPUT_KEYBOARD;
    inp.ki.wScan = scan;
    inp.ki.dwFlags = KEYEVENTF_SCANCODE | (down ? 0 : KEYEVENTF_KEYUP);
    SendInput(1, &inp, sizeof(inp));
}

QString TutlsController::directionToStr(const MovingDirection direction) const
{
    switch(direction)
    {
    case MovingDirection::Forward:
        return "Forward";
    case MovingDirection::Back:
        return "Back";
    case MovingDirection::Right:
        return "Right";
    case MovingDirection::Left:
        return "Left";
    default:
        return "";
    }
}

int TutlsController::directionToVkKey(const MovingDirection direction) const
{
    switch(direction)
    {
    case MovingDirection::Forward:
        return 0x57;
    case MovingDirection::Back:
        return 0x53;
    case MovingDirection::Right:
        return 0x44;
    case MovingDirection::Left:
        return 0x41;
    default:
        return VK_SPACE;
    }
}

TutlsController::TutlsController() { }

void TutlsController::setTutlsProcess(HWND handler)
{
    tutlsHandler_ = handler;
}

bool TutlsController::showTutliWindow() const
{
    if(!tutlsHandler_)
    {
        return false;
    }

    // ShowWindow(tutlsHandler_, SW_RESTORE);
    SetForegroundWindow(tutlsHandler_);
    // UpdateWindow(tutlsHandler_);
    return true;
}

void TutlsController::zoomInMinimap() const
{
    if(!showTutliWindow())
    {
        return;
    }
    log("Send zoom command: '+'");
    SendKyboardEvent(VK_OEM_PLUS, true);
    SendKyboardEvent(VK_OEM_PLUS, false);
}

void TutlsController::zoomOutMinimap() const
{
    if(!showTutliWindow())
    {
        return;
    }
    log("Send zoom command: '-'");
    SendKyboardEvent(VK_OEM_MINUS, true);
    SendKyboardEvent(VK_OEM_MINUS, false);
}

void TutlsController::startMoving(const MovingDirection direction) const
{
    if(!showTutliWindow())
    {
        return;
    }

    log("Send start moving direction command: " + directionToStr(direction).toStdString());

    SendKyboardEvent(directionToVkKey(direction), true);
}

void TutlsController::stopMoving(const MovingDirection direction) const
{
    if(!showTutliWindow())
    {
        return;
    }

    log("Send stop moving direction command: " + directionToStr(direction).toStdString());

    SendKyboardEvent(directionToVkKey(direction), false);
}


/*
// посылает нажатие или отпускание клавиши со сканкодом scan
// если up == TRUE то это событие отпускания, иначе нажатия
// возвращает NO_ERROR в случае успеха или код ошибки
DWORD SendScanCode(WORD scan, BOOL up)
{
    INPUT inp = {0};
    inp.type = INPUT_KEYBOARD;
    inp.ki.wScan = scan;
    inp.ki.dwFlags = KEYEVENTF_SCANCODE | (up ? KEYEVENTF_KEYUP : 0);
    return SendInput(1, &inp, sizeof(inp)) ? NO_ERROR : GetLastError();
}
DWORD SendVirtualKey(UINT vk, BOOL up)
// посылает нажатие или отпускание виртуальной клавиши с кодом vk
// если up == TRUE то это событие отпускания, иначе нажатия
// возвращает NO_ERROR в случае успеха или код ошибки
{
    UINT scan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    // конвертируем виртуальный код в сканкод
    return scan ? SendScanCode(scan, up) : ERROR_NO_UNICODE_TRANSLATION;
    // если конверсия не удалась, то возвращаем
    // ERROR_NO_UNICODE_TRANSLATION, хоть это и не вполне точно описывает
    // произошедшее, но ты будешь знать что трансляция навернулась.
    // Если трансляция есть, то посылаем полученный сканкод
}
DWORD SendChar(TCHAR ch, BOOL up)
// посылает нажатие или отпускание клавиши с кодом символа ch
// если up == TRUE то это событие отпускания, иначе нажатия
// возвращает NO_ERROR в случае успеха или код ошибки
{
    SHORT vk = VkKeyScan(ch);
    // конвертируем символ в код виртуальной клавиши
    DWORD Result;
    if (0xFFFF == (USHORT)vk) // если не вышло возвращаем ошибку
        Result = ERROR_NO_UNICODE_TRANSLATION;
    else
        if (up) // если это отпускание то игнорируем состояние shift, ctrl, alt
            Result = SendVirtualKey(LOBYTE(vk), TRUE);
            // и посылаем отпускание полученного виртуального кода
        else {
            // если это нажатие то нужно учесть shift, ctrl, alt
            // анализируем старший байт и при необходимости посылаем
            // виртуальные коды VK_LSHIFT, VK_LCONTROL, VK_LMENU соответственно
            Result = NO_ERROR;
            if (HIBYTE(vk) & 1) Result = SendVirtualKey(VK_LSHIFT,   FALSE);

            // каждый шаг продолжаем только если нет ошибки, иначе просто возвращаем её код
            if (NO_ERROR == Result) {
            if (HIBYTE(vk) & 2) Result = SendVirtualKey(VK_LCONTROL, FALSE);

            if (NO_ERROR == Result) {
            if (HIBYTE(vk) & 4) Result = SendVirtualKey(VK_LMENU,    FALSE);

            if (NO_ERROR == Result) {
            Result = SendVirtualKey(LOBYTE(vk), FALSE);
            // посылаем нажатиме полученного виртуального кода

            // снова анализируем старший байт и при необходимости посылаем отпускания виртуальных
            // кодов VK_LSHIFT, VK_LCONTROL, VK_LMENU, нажатия которых мы послали выше
            if (NO_ERROR == Result) {
            if (HIBYTE(vk) & 4) Result = SendVirtualKey(VK_LMENU,    TRUE);

            if (NO_ERROR == Result) {
            if (HIBYTE(vk) & 2) Result = SendVirtualKey(VK_LCONTROL, TRUE);

            if (NO_ERROR == Result) {
            if (HIBYTE(vk) & 1) Result = SendVirtualKey(VK_LSHIFT,   TRUE);
            }}}}}}
        }
    return Result;
}
DWORD SendString(LPCTSTR s)
// посылает последовательность нажатий/отпусканий символов строки s
// возвращает NO_ERROR в случае успеха или код ошибки
{
    DWORD Result = NO_ERROR;
    for (; *s && !Result; ++s) {
    // идем по всем символам строки пока не получили ошибку
        Result = SendChar(s[0], FALSE);
        // посылаем нажатие каждого символа
        if (NO_ERROR == Result) Result = SendChar(s[0], TRUE);
        // и если нет ошибки, то его отпускание
    }
    return Result;
}
*/
