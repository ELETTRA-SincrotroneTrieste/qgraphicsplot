#include <QApplication>
#include "xygscene.h"
#include "../../src/plotscenewidget.h"


int main(int argc, char *argv[])
{
    /* Create QApplication */
    QApplication a( argc, argv );

    a.setApplicationName("Spectrum");
    a.setOrganizationName("QGraphicsPlot");


    XYSceneTest w;

    w.show();
    
    return a.exec();
}
