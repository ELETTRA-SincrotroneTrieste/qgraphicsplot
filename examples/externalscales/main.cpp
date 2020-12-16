#include <QApplication>
#include "externalscales.h"
#include "../../src/plotscenewidget.h"


int main(int argc, char *argv[])
{
    /* Create QApplication */
    QApplication a( argc, argv );

    a.setApplicationName("ExternalScales");
    a.setOrganizationName("QGraphicsPlot");


    ExternalScales w;

    w.show();
    
    return a.exec();
}
