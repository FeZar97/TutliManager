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
    void startBattle();
    void startProcess();
};

#endif // WIDGET_H
