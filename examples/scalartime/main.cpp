#include <QApplication>
#include "scalartime.h"
#include "../../src/plotscenewidget.h"


int main(int argc, char *argv[])
{
    /* Create QApplication */
    QApplication a( argc, argv );

    a.setOrganizationName("QGraphicsPlot");
    a.setApplicationName("ScalarTime");


    XYSceneTest w;

    w.show();
    
    return a.exec();
}
