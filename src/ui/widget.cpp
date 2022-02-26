#include <QGridLayout>
#include "widget.h"

#include "src/core/logger.h"

Widget::Widget(QWidget *parent): QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    setFixedSize(227, 227);

    wrk_thread.start();
    wrk.moveToThread(&wrk_thread);
    connect(this, &Widget::startProcess, &wrk, &WORKER::startProcess);

    connect(&wrk, &WORKER::setMapParams, this, &Widget::updateUiParams);

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

    Logger::log("Set ui params: size(" + std::to_string(newSize) + "), name: " + newMapName.toStdString());

    // set size
    QSize newWidgetSize{newSize, newSize};
    if (size() != newWidgetSize)
    {
        setFixedSize(newWidgetSize);
    }

    // set map
    setWindowTitle(newMapName);
}
