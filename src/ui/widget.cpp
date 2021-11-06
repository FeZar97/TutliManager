#include "widget.h"

Widget::Widget(QWidget *parent): QWidget(parent)
{
    setFixedSize(228, 48);
    wrk_thread.start();
    wrk.moveToThread(&wrk_thread);
    connect(this, &Widget::startBattle,            &wrk,   &WORKER::startBattle);
}

Widget::~Widget()
{
    wrk_thread.quit();
    wrk_thread.wait();
}
