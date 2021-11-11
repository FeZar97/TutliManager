#include <QGridLayout>
#include "widget.h"

Widget::Widget(QWidget *parent): QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    resize(400, 60);

    wrk_thread.start();
    wrk.moveToThread(&wrk_thread);
    connect(this, &Widget::startBattle, &wrk, &WORKER::startBattle);
    connect(this, &Widget::startProcess, &wrk, &WORKER::startProcess);

    emit startProcess();
}

Widget::~Widget()
{
    wrk.enableWork = false;
    wrk_thread.quit();
    wrk_thread.wait();
}
