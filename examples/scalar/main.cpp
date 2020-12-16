#include <QApplication>
#include "scalar.h"
#include "../../src/plotscenewidget.h"


int main(int argc, char *argv[])
{
    /* Create QApplication */
    QApplication a( argc, argv );

    a.setApplicationName("Spectrum");


    XYSceneTest w;

    w.show();
    
    return a.exec();
}
