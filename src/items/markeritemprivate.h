#ifndef MARKERITEMPRIVATE_H
#define MARKERITEMPRIVATE_H

#include <QPointF>
#include <QRectF>
#include <QList>
#include <QColor>

class SceneCurve;
class PlotSceneWidget;

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

    PlotSceneWidget *plot;
};

#endif // MARKERITEMPRIVATE_H

