######################################################################
# Automatically generated by qmake (2.01a) Fri Mar 16 11:25:33 2012
######################################################################

TEMPLATE = app
TARGET = scalartime
DEPENDPATH += .
INCLUDEPATH += . ../../src ../../src/curve ../../src/axes

QMAKE_CXXFLAGS += -O2

DEFINES -= QT_NO_DEBUG_OUTPUT

# Input
HEADERS += \
    scalartime.h
FORMS += \
    scalar.ui
SOURCES += main.cpp \
    scalartime.cpp

LIBS += -L../.. -lQGraphicsPlot
