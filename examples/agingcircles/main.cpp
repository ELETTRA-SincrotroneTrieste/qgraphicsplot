#include <QtGui/QApplication>
#include "agingcircles.h"
#include "../../src/plotscenewidget.h"


int main(int argc, char *argv[])
{
    /* Create QApplication */
    QApplication a( argc, argv );

    a.setApplicationName("AgingCircles");


    AgingCircles w;

    w.show();
    
    return a.exec();
}
