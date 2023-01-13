#ifndef TARGETITEMPRIVATE_H
#define TARGETITEMPRIVATE_H

#include <QColor>
#include <QPointF>

class ItemMoveListener;
class QGraphicsPlotItem;
class ScaleItem;

class TargetItemPrivate
{
public:
    TargetItemPrivate();

    qreal radius;

    QColor circleColor;

    ItemMoveListener *itemMoveListener;

    bool mousePressed, pointLabelEnabled;

    double textHeight, textWidth;

    double innerRadius, outerRadius, radiusScaleDivider;

    QGraphicsPlotItem *plot;

    ScaleItem* xAxis, *yAxis;

    double x, y;
};

#endif // TARGETITEMPRIVATE_H
