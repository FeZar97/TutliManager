#include <QGridLayout>
#include <QPainter>
#include "widget.h"

#include "src/core/logger.h"

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, curMapPixmap_.width(), curMapPixmap_.height(), curMapPixmap_);
}

Widget::Widget(QWidget *parent): QWidget(parent)
{
    mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    wrk_thread.start();
    wrk.moveToThread(&wrk_thread);
    connect(this, &Widget::startProcess, &wrk, &WORKER::startProcess);
    connect(&wrk, &WORKER::setMapParams, this, &Widget::updateUiParams);
    connect(&wrk, &WORKER::plotCurMapPixmap, this, &Widget::updateCurMapPixmap);

    moveControllerWidget = new MoveControllerWidget(this);
    connect(moveControllerWidget, &MoveControllerWidget::moveSig, [this](const MovingDirection direction, const bool active){
        wrk.onMoveRecieved(direction, active);
    });
    mainLayout->addWidget(moveControllerWidget);

    setFixedSize(227, 227);

    emit startProcess();
}

Widget::~Widget()
{
    wrk.enableWork = false;
    wrk_thread.quit();
    wrk_thread.wait();
}

void Widget::updateUiParams(const MapSizeIdx mapSize, const MapName mapName)
{
    int newSize = MapSizeDetector::mapIdxToSize(mapSize);
    QString newMapName = QString::fromStdString(MapNameDetector::mapNameToStr(mapName));

    // Logger::log("Set ui params: size(" + std::to_string(newSize) + "), name: " + newMapName.toStdString());

    // set size
    QSize newWidgetSize{newSize, newSize};
    if (size() != newWidgetSize)
    {
        setFixedSize(newWidgetSize);
    }

    // set map
    setWindowTitle(newMapName);
}

void Widget::updateCurMapPixmap(const QPixmap curMapPixmap)
{
    curMapPixmap_ = curMapPixmap;
    repaint();
}
