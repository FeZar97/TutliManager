#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent): QWidget(parent),
                                 ui(new Ui::Widget)
{
    ui->setupUi(this);

    wrk_thread.start();
    wrk.moveToThread(&wrk_thread);
    connect(this, &Widget::startTutlsEnumerate,    &wrk,   &WORKER::enumerateTanks);
    connect(this, &Widget::startBattle,            &wrk,   &WORKER::startBattle);
    connect(&wrk, &WORKER::showScreen,             this,   &Widget::showScreen);

    connect(this, &Widget::makeScreenCurrentTank,  &wrk,   &WORKER::screenCurrentTank);
    connect(&wrk, &WORKER::showCurrentTank,        this,   &Widget::showCurrentTank);

    cvNamedWindow(winName, 0);

    // текущий танк
    cvNamedWindow(currentTankWin, 0);
    cvResizeWindow(currentTankWin, TANK_SELECT_REGION_WIDTH, TANK_SELECT_REGION_HEIGHT);
    cvMoveWindow(currentTankWin, 0, 0);
}

Widget::~Widget()
{
    cvReleaseImage(&iimg);
    cvDestroyWindow(winName);

    // текущий танк
    cvReleaseImage(&curTankImg);
    cvDestroyWindow(currentTankWin);
    delete ui;
}

// запуск перебора
void Widget::on_TutlChooseButton_clicked()
{
    emit startTutlsEnumerate(ui->tutlNumberSB->value());
}

// васьки в бой
void Widget::on_GoToBattleButton_clicked()
{
    emit startBattle();
}

// индекс размера карты
void Widget::on_mapSizeIdxSB_valueChanged(int _mapSizeIdx)
{
    wrk.mapSizeIdx = _mapSizeIdx;
}

// отображение
void Widget::showScreen()
{
    iimg = converter::QImageToIplImage(&wrk.lastScreen);
    cvShowImage(winName, iimg);
}

// отображение текущего танка
void Widget::showCurrentTank()
{
    curTankImg = converter::QImageToIplImage(&wrk.currentTankImage);
    cvShowImage(currentTankWin, curTankImg);
}
