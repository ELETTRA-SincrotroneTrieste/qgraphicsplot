#ifndef QGRAPHICSPLOTITEMPRIVATE_H
#define QGRAPHICSPLOTITEMPRIVATE_H

#include <QHash>
#include <QMap>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QColor>

class QObject;
class Point;
class SceneCurve;
class QGraphicsPlotItem;
class AxesManager;
class PlotGeometryEventListener;
class MouseEventListener;
class QGraphicsZoomer;
class LegendItem;
class QMenu;

class QGraphicsPlotItemPrivate
{
public:
    QGraphicsPlotItemPrivate();

    bool scaleOnResize, scaleOnScroll, mouseZoomEnabled;
    bool modifiedPaintEvent, scrollBarsEnabled;
    bool neverShown, sceneRectToWidgetGeometry;

    bool useGl;

    QMap<QString, QObject*> configurableObjectsMap;

    QString settingsKey;

    QHash<QString, SceneCurve *> curveHash;

    AxesManager *axesManager;

    double topLeftXPercent, topLeftYPercent, widthPercent, heightPercent;

    double firstScrollM11, firstScrollM12;

    /* cache the plot rect for efficiency */
    QRectF plotRect, zoomArea;

    QList<PlotGeometryEventListener *> plotGeometryEventListeners;

    QList<MouseEventListener *> mouseEventListeners;

    bool mousePressed, mouseMoving;

    QPointF mousePressedPoint, mouseMovingPoint;

    QGraphicsZoomer * zoomer;

    LegendItem *legendItem;

    QMenu *menu;

    QColor backgroundColor;

    /// TEST
    QRectF updateRect;

private:
    QGraphicsPlotItem *mView;


};

#endif // XYSCENEPRIVATE_H
