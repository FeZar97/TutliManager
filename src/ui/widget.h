#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <QTimer>
#include <QPaintEvent>

#include "src/core/worker.h"
#include "MoveControllerWidget.h"


class Widget : public QWidget
{
    Q_OBJECT

    QGridLayout *mainLayout;
    MoveControllerWidget *moveControllerWidget;
    QPixmap curMapPixmap_;

protected:
    void paintEvent(QPaintEvent*) override;

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    WORKER   wrk;
    QThread  wrk_thread;

public slots:
    void updateUiParams(const MapSizeIdx mapSize, const MapName mapName);
    void updateCurMapPixmap(const QPixmap curMapPixmap);

signals:
    void startProcess();
};

#endif // WIDGET_H
