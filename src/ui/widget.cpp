#include "widget.h"

Widget::Widget(QWidget *parent): QWidget(parent)
{
    wrk_thread.start();
    wrk.moveToThread(&wrk_thread);
    connect(this, &Widget::startTutlsEnumerate,    &wrk,   &WORKER::enumerateTanks);
    connect(this, &Widget::startBattle,            &wrk,   &WORKER::startBattle);
}

Widget::~Widget()
{
    wrk_thread.quit();
    wrk_thread.wait();
}
