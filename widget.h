#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <QTimer>

#include <worker.h>

const static char* winName{"tutlVasek"};
const static char* currentTankWin{"current tutl"};

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    WORKER   wrk;
    QThread  wrk_thread;
    QImage   *qimg;
    IplImage *iimg;
    IplImage *curTankImg;


private slots:
    void on_TutlChooseButton_clicked(); // запуск перебора
    void on_GoToBattleButton_clicked(); // васьки в бой
    void on_mapSizeIdxSB_valueChanged(int _mapSizeIdx); // индекс размера карты

public slots:
    void showScreen();
    void showCurrentTank();

signals:
    void startTutlsEnumerate(int _tutlsNumber);
    void makeScreenCurrentTank();
    void startBattle();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
