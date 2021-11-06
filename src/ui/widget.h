#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <QTimer>

#include "src/core/worker.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    WORKER   wrk;
    QThread  wrk_thread;

signals:
    void startTutlsEnumerate(int _tutlsNumber);
    void startBattle();
};

#endif // WIDGET_H
