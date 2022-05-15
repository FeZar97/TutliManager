#pragma once

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QResizeEvent>

#include "src/core/Helper.h"

class MoveControllerWidget : public QWidget
{
    Q_OBJECT

    QGridLayout *mainLayout;

    QPushButton *wButton;
    QPushButton *dButton;
    QPushButton *aButton;
    QPushButton *sButton;

    QMap<MovingDirection, bool> directionActions;

protected:
    void resizeEvent(QResizeEvent *event) override;

public:
    explicit MoveControllerWidget(QWidget *parent = nullptr);
    ~MoveControllerWidget() = default;

signals:
    void moveSig(const MovingDirection direction, const bool active);
};
