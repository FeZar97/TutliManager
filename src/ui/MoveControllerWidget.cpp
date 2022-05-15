#include "MoveControllerWidget.h"
#include <QQmlEngine>
#include <QDebug>

void MoveControllerWidget::resizeEvent(QResizeEvent *event)
{
    event->accept();
    setFixedSize(event->size());
    const QSize newButtonsSize = {event->size().width()/3, event->size().height()/3};
    wButton->setFixedSize(newButtonsSize);
    dButton->setFixedSize(newButtonsSize);
    aButton->setFixedSize(newButtonsSize);
    sButton->setFixedSize(newButtonsSize);
}

MoveControllerWidget::MoveControllerWidget(QWidget *parent): QWidget(parent)
{
    qRegisterMetaType<MovingDirection>("MovingDirection");
    mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    wButton = new QPushButton("W", nullptr);
    wButton->setFixedSize({20, 20});
    dButton = new QPushButton("D", nullptr);
    dButton->setFixedSize({20, 20});
    aButton = new QPushButton("A", nullptr);
    aButton->setFixedSize({20, 20});
    sButton = new QPushButton("S", nullptr);
    sButton->setFixedSize({20, 20});

    mainLayout->addWidget(wButton, 0, 1);
    mainLayout->addWidget(aButton, 1, 0);
    mainLayout->addWidget(dButton, 1, 2);
    mainLayout->addWidget(sButton, 2, 1);

    directionActions[MovingDirection::Forward] = false;
    directionActions[MovingDirection::Right] = false;
    directionActions[MovingDirection::Left] = false;
    directionActions[MovingDirection::Back] = false;

    connect(wButton, &QPushButton::clicked, [this](){
        bool curState = !directionActions[MovingDirection::Forward];
        directionActions[MovingDirection::Forward] = curState;
        wButton->setChecked(curState);
        emit this->moveSig(MovingDirection::Forward, curState);
    });

    connect(dButton, &QPushButton::clicked, [this](){
        bool curState = !directionActions[MovingDirection::Right];
        directionActions[MovingDirection::Right] = curState;
        dButton->setChecked(curState);
        emit moveSig(MovingDirection::Right, curState);
    });

    connect(aButton, &QPushButton::clicked, [this](){
        bool curState = !directionActions[MovingDirection::Left];
        directionActions[MovingDirection::Left] = curState;
        aButton->setChecked(curState);
        emit moveSig(MovingDirection::Left, curState);
    });

    connect(sButton, &QPushButton::clicked, [this](){
        bool curState = !directionActions[MovingDirection::Back];
        directionActions[MovingDirection::Back] = curState;
        sButton->setChecked(curState);
        emit moveSig(MovingDirection::Back, curState);
    });
}
