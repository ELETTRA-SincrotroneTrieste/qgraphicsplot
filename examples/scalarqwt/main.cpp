#include <QtGui/QApplication>
#include "qwtplotw.h"


int main(int argc, char *argv[])
{
    /* Create QApplication */
    QApplication a( argc, argv );

    a.setApplicationName("Spectrum");


    EPlotLightTest w;

    w.show();
    
    return a.exec();
}
