#include "point.h"
#include "pointprivate.h"
#include "qgraphicsplotitem.h"
#include "itempainterinterface.h"
#include "itempositionchangelistener.h"

#include <QtDebug>
#include "../qgraphicsplotmacros.h"
#include <stdio.h>

Point::~Point()
{

}

Point::Point(double _x, double _y)
{
    x = _x;
    y = _y;
}

Point::Point(const PointData &data, ScaleItem *xAxis, ScaleItem *yAxis)
{
    d_ptr = new PointPrivate(data);
    d_ptr->xAxis = xAxis;
    d_ptr->yAxis = yAxis;
}

const PointData & Point::pointDataR() const
{
    return d_ptr->data;
}

PointData Point::pointData() const
{
    return d_ptr->data;
}

void Point::setPointData(const PointData& pd)
{
    d_ptr->data = pd;
}

void Point::setPos(const QPointF &pos)
{
    d_ptr->pos = pos;
}

QPointF Point::pos() const
{
    return d_ptr->pos;
}

void Point::setPos(qreal x, qreal y)
{
    foreach(ItemPositionChangeListener *ipcl, d_ptr->itemPositionChangeListeners)
        ipcl->onItemPositionChanged(QPointF(x, y));
    d_ptr->pos = QPointF(x, y);
}

double Point::updateXPos(QGraphicsPlotItem *view)
{
    double xp = 0.0;
    double x1 = d_ptr->xAxis->lowerBound();
    double x2 = d_ptr->xAxis->upperBound();
    double x = d_ptr->data.x;
    if(x2 == x1)
        return xp;
    QRectF plotRect = view->plotRect();
    xp = plotRect.width() * (x - x1) / (x2 - x1) + plotRect.left();
    setPos(xp, d_ptr->pos.y());
    return xp;
}

double Point::updateYPos(QGraphicsPlotItem *view)
{
    double yp = 0.0;
    double y1 = d_ptr->yAxis->lowerBound();
    double y2 = d_ptr->yAxis->upperBound();
    double y = d_ptr->data.y;
    if(y1 == y2)
        return yp;

    QRectF plotRect = view->plotRect();
    yp = plotRect.height() - (plotRect.height() * (y - y1) / (y2 - y1) + plotRect.top());
    setPos(d_ptr->pos.x(), yp);
    return yp;
}

QPointF Point::updatePosition(QGraphicsPlotItem *view)
{
    QPointF pt(0,0);

    double x1 = d_ptr->xAxis->lowerBound();
    double x2 = d_ptr->xAxis->upperBound();
    double y1 = d_ptr->yAxis->lowerBound();
    double y2 = d_ptr->yAxis->upperBound();

    double x = d_ptr->data.x;
    double y = d_ptr->data.y;

    if(x2 == x1 || y2 == y1)
        return pt;

    QRectF plotRect = view->plotRect();
    qreal px = plotRect.width() * (x - x1) / (x2 - x1) + plotRect.left();
    qreal py = plotRect.height() - (plotRect.height() * (y - y1) / (y2 - y1) + plotRect.top());
    pt = QPointF(px, py);

    d_ptr->pos = pt;

    foreach(ItemPositionChangeListener *ipcl, d_ptr->itemPositionChangeListeners)
        ipcl->onItemPositionChanged(pt);

    return pt;
}
