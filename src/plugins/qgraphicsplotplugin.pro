# change to your desired install root directory

message("qgraphicsplot plugins: parameters for qmake: prefix")
message("                       for example: qmake prefix=/usr/local/qgraphicsplot/plugins")

isEmpty(prefix) {
	INSTALL_ROOT = /runtime
} else {
	INSTALL_ROOT = $${prefix}
}

message("qgraphicsplot plugins: installation dir (prefix): $${INSTALL_ROOT}")

# change to your desired lib dir
LIB_DIR = $${INSTALL_ROOT}/lib

QT += xml designer

greaterThan(QT_MAJOR_VERSION, 4) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
    QT += designer
} else {
    CONFIG += designer
    QTVER_SUFFIX =
}

CONFIG += plugin

DESTDIR = ./designer

SOURCES	+= qgraphicsplotplugin.cpp
HEADERS	+= qgraphicsplotplugin.h
RESOURCES += qgraphicsplotplugin.qrc

TEMPLATE = lib

TARGET = qgraphicsplot$${QTVER_SUFFIX}plugin

QMAKE_CLEAN += designer/qgraphicsplotplugin*

CONFIG	+= qt thread warn_on

unix:LIBS += -L../.. -lQGraphicsPlot$${QTVER_SUFFIX}

# the same goes for INCLUDEPATH
INCLUDEPATH -= $${INC_DIR}
INCLUDEPATH += .. ../items ../axes

plugin.commands = ln -sf $${LIB_DIR}/plugins/designer/libqgraphicsplot$${QTVER_SUFFIX}plugin.so $${LIB_DIR}/libqgraphicsplot$${QTVER_SUFFIX}plugin.so
plugin.files = designer/libqgraphicsplot$${QTVER_SUFFIX}plugin*
plugin.path = $${LIB_DIR}/plugins/designer

INSTALLS += plugin
