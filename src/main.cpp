#include <QApplication>
#include "src/ui/widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<MapSizeIdx>("MapSizeIdx");
    qRegisterMetaType<MapName>("MapName");

    Widget w;
    w.show();

    return a.exec();
}
