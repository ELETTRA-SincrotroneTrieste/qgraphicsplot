#ifndef PLOTSCENEWIDGETPRIVATE_H
#define PLOTSCENEWIDGETPRIVATE_H

#include <QMap>
#include "scaleitem.h"

class Point;
class SceneCurve;
class PlotSceneWidget;
class AxesManager;
class PlotGeometryEventListener;
class MouseEventListener;
class QGraphicsZoomer;
class LegendItem;

class PlotSceneWidgetPrivate
{
public:
    PlotSceneWidgetPrivate(PlotSceneWidget *view);

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
    QRectF plotRect;

    QList<PlotGeometryEventListener *> plotGeometryEventListeners;

    QList<MouseEventListener *> mouseEventListeners;

    bool mousePressed, mouseMoving;

    QPointF mousePressedPoint, mouseMovingPoint;

    QGraphicsZoomer * zoomer;

    LegendItem *legendItem;

private:
    PlotSceneWidget *mView;


};

#endif // XYSCENEPRIVATE_H
