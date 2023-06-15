
###### MOVED TO cumbia-qtcontrols-ng

error("qgraphicsplot library has moved to cumbia-qtcontrols-ng: https://github.com/ELETTRA-SincrotroneTrieste/cumbia-qtcontrols-ng")

# + ----------------------------------------------------------------- +
#
# Customization section:
#
# Customize the following paths according to your installation:
#
# change to where you want the libraries to be installed:
#
isEmpty(prefix) {
    INSTALL_ROOT = /usr/local/qgraphicsplot
    message("QGraphicsPlot: *** call qmake prefix=/usr/local/qgraphicsplot to change the installation prefix. ***")
} else {
    INSTALL_ROOT = $${prefix}
}

message("QGraphicsPlot: *** installation dir: \"$${INSTALL_ROOT}\". ***")

libname=qgraphicsplot

#
# End customization
#
# + ----------------------------------------------------------------- +
#

contains(QT_VERSION,  "^4\\.[0-7]") {
        error("Use at least Qt 4.8. If you want to disable this error, modify this line in qgraphicsplot.pro")
}

# append a "-qt6" suffix to the library built with qt5
greaterThan(QT_MAJOR_VERSION, 5) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
} else {
    QTVER_SUFFIX =
}


# change to your desired lib dir
LIB_DIR = $${INSTALL_ROOT}/lib

# change the place where you want the include files to be installed
INC_DIR = $${INSTALL_ROOT}/include/qgraphicsplot

# change the place where you want the documentation and other files to be installed
SHAREDIR = $${INSTALL_ROOT}/share

# documentation directory. Inside DOC_DIR you will find a doc directory
DOC_DIR = $${SHAREDIR}/qgraphicsplot

# this is where the QT documentation is installed, if you want Doxygen to
# generate links to Qt documentation
QT_DOC_DIR =

QT += opengl widgets

TEMPLATE = lib

#DEFINES += DEBUG_PAINT=1

CONFIG += debug

DEFINES += QT_NO_DEBUG_OUTPUT


VERSION_HEX = 0x030000
VERSION = 3.0.0
VER_MAJ = 3
VER_MIN = 0
VER_FIX = 0

DEFINES += QGRAPHICSPLOT_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    QGRAPHICSPLOT_VERSION=$${VERSION_HEX} \
    VER_MAJ=$${VER_MAJ} \
    VER_MIN=$${VER_MIN} \
    VER_FIX=$${VER_FIX} \
    SHAREDIR=\"\\\"$${SHAREDIR}\\\"\"  \
    DOCDIR=\"\\\"$${DOC_DIR}\\\"\"  \
    INSTALLROOT=\"\\\"$${INSTALL_ROOT}\\\"\"  \
    LIBDIR=\"\\\"$${LIBDIR}\\\"\"

TARGET = $${libname}$${QTVER_SUFFIX}

QMAKE_CXXFLAGS += -O3

MOC_DIR = moc
OBJECTS_DIR = obj
FORMS_DIR = ui
LANGUAGE = C++
UI_DIR = src

docs.commands = doxygen \
    Doxyfile;

DEPENDPATH += . \
              src \
              src/axes \
              src/circles \
              src/curve \
              src/properties \
              src/curve/items
INCLUDEPATH += . \
               src \
               src/axes \
               src/curve \
               src/properties \
               src/circles \
               src/curve/items

# Input
PUBLICHEADERS= src/qgraphicsplotitem.h \
            src/plotscenewidget.h \
           src/xyplotinterface.h \
           src/axes/axesmanager.h \
           src/axes/axiscouple.h \
           src/axes/scaleitem.h \
           src/curve/curvechangelistener.h \
           src/curve/itempainterinterface.h \
           src/curve/itempositionchangelistener.h \
           src/curve/curveitem.h \
           src/curve/pointdata.h \
           src/curve/scenecurve.h \
           src/curve/point.h \
           src/properties/propertydialog.h \
           src/properties/settingsloader.h \
           src/axischangelistener.h \
           src/mouseeventlistener.h \
           src/items/markeritem.h \
           src/items/targetitem.h \
    src/items/itemmovelistener.h \
    src/curve/painters/dotspainter.h \
    src/curve/painters/linepainter.h \
    src/curve/painters/stepspainter.h \
    src/curve/data.h \
           src/curve/painters/circleitemset.h \
    src/curve/painters/histogrampainter.h \
    src/scalelabelinterface.h \
    src/scalelabels/timescalelabel.h \
    src/qgraphicszoomer.h \
    src/externalscalewidget.h \
    src/horizontalscalewidget.h \
    src/items/legenditem.h \
    src/verticalscalewidget.h \
    src/plotgeometryeventlistener.h \
    src/qgraphicsplotmacros.h \
    src/extscaleplotscenewidget/extscaleplotscenewidget.h \
    src/extscaleplotscenewidget/curvesmap.h

HPRIVATES = src/qgraphicsplotitem_private.h \
           src/axes/scaleitemprivate.h \
           src/curve/curveitemprivate.h \
           src/curve/scenecurveprivate.h \
           src/curve/pointprivate.h

HEADERS += $${HPRIVATES} \
    src/items/markeritemprivate.h \
    src/curve/painters/linepainterprivate.h \
    src/curve/painters/dotspainterprivate.h \
    src/items/targetitemprivate.h \
    src/curve/painters/histogrampainterprivate.h \
    src/colorpalette.h \
    src/plotsaver/plotscenewidgetsaver.h \
    src/curve/painters/stepspainterprivate.h \
    src/plotscenewidget.h

HEADERS += $${PUBLICHEADERS}

SOURCES += \
    src/plotscenewidget.cpp \
           src/axes/axesmanager.cpp \
           src/axes/axiscouple.cpp \
           src/axes/scaleitem.cpp \
           src/axes/scaleitemprivate.cpp \
           src/curve/itempositionchangelistener.cpp \
           src/curve/pointdata.cpp \
           src/curve/scenecurve.cpp \
           src/curve/scenecurveprivate.cpp \
           src/properties/propertydialog.cpp \
           src/properties/settingsloader.cpp \
    src/curve/point.cpp \
    src/curve/pointprivate.cpp \
    src/curve/data.cpp \
    src/curve/curveitem.cpp \
    src/curve/curveitemprivate.cpp \
    src/items/markeritem.cpp \
    src/items/markeritemprivate.cpp \
    src/curve/painters/linepainter.cpp \
    src/curve/painters/linepainterprivate.cpp \
    src/curve/painters/dotspainterprivate.cpp \
    src/curve/painters/dotspainter.cpp \
    src/items/targetitem.cpp \
    src/items/targetitemprivate.cpp \
    src/items/itemmovelistener.cpp \
    src/curve/painters/circleitemset.cpp \
    src/curve/painters/histogrampainter.cpp \
    src/curve/painters/histogrampainterprivate.cpp \
    src/qgraphicsplotitem.cpp \
    src/qgraphicsplotitem_private.cpp \
    src/scalelabelinterface.cpp \
    src/scalelabels/timescalelabel.cpp \
    src/qgraphicszoomer.cpp \
    src/externalscalewidget.cpp \
    src/horizontalscalewidget.cpp \
    src/verticalscalewidget.cpp \
    src/colorpalette.cpp \
    src/items/legenditem.cpp \
    src/plotsaver/plotscenewidgetsaver.cpp \
    src/curve/painters/stepspainter.cpp \
    src/curve/painters/stepspainterprivate.cpp \
    src/extscaleplotscenewidget/curvesmap.cpp

inc.files = $${PUBLICHEADERS}
inc.path = $${INC_DIR}

target.path=$${LIB_DIR}


doc.commands = \
    doxygen \
    Doxyfile;

doc.files = doc/
doc.path = $${DOC_DIR}

data.path       = $${SHAREDIR}/qgraphicsplot/data
data.files = src/plotsaver/saveDataFormatHelp.html

# generate pkg config file
CONFIG += create_pc create_prl no_install_prl

QMAKE_PKGCONFIG_NAME = $${libname}
QMAKE_PKGCONFIG_DESCRIPTION = plot items to use in QGraphicsView
QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
QMAKE_PKGCONFIG_LIBDIR = $${target.path}
QMAKE_PKGCONFIG_INCDIR = $${inc.path}
QMAKE_PKGCONFIG_VERSION = $${VERSION}
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

pkgconfig_f.path = $${LIB_DIR}/pkgconfig
pkgconfig_f.files = pkgconfig/$${libname}.pc

# INSTALLS = inc lib doc data

INSTALLS = inc target data pkgconfig_f

OTHER_FILES += \
    INSTALL.txt



