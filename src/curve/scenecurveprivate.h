#ifndef SCENECURVEPRIVATE_H
#define SCENECURVEPRIVATE_H

class QGraphicsPlotItem;
class Point;
class ScaleItem;
class CurveChangeListener;
class CurveItem;

#include <QList>
#include <QPolygon>
#include <QString>
#include <QColor>
#include <QVector>
#include <QRectF>
#include "data.h"

class SceneCurvePrivate
{
public:
    SceneCurvePrivate();

    QGraphicsPlotItem *plot;

    int bufferSize;

    QList<Point *> points;

    QString name;

    ScaleItem *xAxis, *yAxis;

    QList<CurveChangeListener *>itemChangeListeners;

    /* The  CurveItem that is used by the curve on the
     * scene.
     */
    CurveItem * curveItem;

    Data *data;

    double xub, xlb, yub, ylb, xextension, yextension;

    int lastValidXPos, lastValidYPos;

    double canvasRectTop, canvasRectW , canvasRectH, canvasRectLeft;

    QVector<QPointF> mPoints;

    QPolygon polygon;
};

#endif // SCENECURVEPRIVATE_H
