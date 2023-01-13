# append a "-qt6" suffix to the library built with qt5
greaterThan(QT_MAJOR_VERSION, 5) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
} else {
    QTVER_SUFFIX =
}


INCLUDEPATH += ../src ../../src ../../../src 
INCLUDEPATH += ../../src ../../src/curve ../../src/axes
INCLUDEPATH += ../../../src ../../../src/curve ../../../src/axes ../../../src/items

LIBS += -L.. -L../.. -L../../.. -lqgraphicsplot$${VER_SUFFIX}

TEMPLATE = subdirs
SUBDIRS = agingcircles scalar spectrum externalscales  scalartime
CONFIG += ordered
