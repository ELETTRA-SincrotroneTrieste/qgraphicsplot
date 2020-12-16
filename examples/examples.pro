contains(QT_VERSION, ^5\\..*\\..*) {
    VER_SUFFIX = -qt5
    QT += widgets
}

INCLUDEPATH += ../src ../../src ../../../src 
INCLUDEPATH += ../../src ../../src/curve ../../src/axes
INCLUDEPATH += ../../../src ../../../src/curve ../../../src/axes ../../../src/items

LIBS += -L.. -L../.. -L../../.. -lQGraphicsPlot$${VER_SUFFIX}

TEMPLATE = subdirs
SUBDIRS = agingcircles scalar spectrum externalscales  scalartime
CONFIG += ordered
