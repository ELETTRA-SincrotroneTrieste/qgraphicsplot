#ifndef POINTPRIVATE_H
#define POINTPRIVATE_H

#include "pointdata.h"

class ItemPainterInterface;
class ScaleItem;
class ItemPositionChangeListener;

#include <QList>
#include <QBrush>
#include <QPen>

class PointPrivate
{
public:
    PointPrivate(const PointData &pointData);

    PointData data;

    int curveId;

    ScaleItem *xAxis, *yAxis;

    QList<ItemPositionChangeListener *> itemPositionChangeListeners;

    QPointF pos;
};

#endif // XYITEMPRIVATE_H
