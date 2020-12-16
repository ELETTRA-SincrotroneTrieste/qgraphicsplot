#ifndef CURVEITEMPRIVATE_H
#define CURVEITEMPRIVATE_H

class SceneCurve;

#include <QRectF>
#include <QPen>
#include <QList>

class ItemPainterInterface;

class CurveItemPrivate
{
public:
    CurveItemPrivate();

    SceneCurve *curve;

    QRectF updateRect, updateRectDelete;

    QList<ItemPainterInterface *>itemPainters;

};

#endif // CURVEITEMPRIVATE_H
