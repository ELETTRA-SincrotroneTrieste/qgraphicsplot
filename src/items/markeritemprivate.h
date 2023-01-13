#ifndef MARKERITEMPRIVATE_H
#define MARKERITEMPRIVATE_H

#include <QPointF>
#include <QRectF>
#include <QList>
#include <QColor>

class SceneCurve;
class QGraphicsPlotItem;

class MarkerItemPrivate
{
public:
    MarkerItemPrivate();

    QPointF closestPoint;

    QRectF boundingRect;

    QList<SceneCurve *>closestCurves;

    int closestIndex;

    double radius;

    bool decorationEnabled;

    QColor borderColor, backgroundColor, textColor, pointBorderColor, pointColor;

    QGraphicsPlotItem *plot;
};

#endif // MARKERITEMPRIVATE_H

