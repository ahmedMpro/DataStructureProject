
#include "gui_qt/graphwindow.h"
#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    GraphWindow w;
    w.resize(1080, 720);
    w.show();
    return app.exec();
}
