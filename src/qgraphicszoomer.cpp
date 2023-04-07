#include "qgraphicszoomer.h"
#include <scaleitem.h>
#include <qgraphicsplotitem.h>
#include <QList>
#include <QPair>
#include <QRectF>
#include <QPointF>
#include <QtDebug>
#include <QSet>
#include <QMap>
#include <QLine>
#include "qgraphicsplotmacros.h"

class QGraphicsZoomerPrivate
{
public:
    QGraphicsPlotItem* plot;
    bool inZoom;

    QSet<ScaleItem *> scaleItems;
    QMap<ScaleItem *, QList<QPointF> > zoomStackMap;
};


QGraphicsZoomer::~QGraphicsZoomer()
{
    foreach(ScaleItem *si, d_ptr->zoomStackMap.keys())
        si->removeAxisChangeListener(this);
}

QGraphicsZoomer::QGraphicsZoomer(QGraphicsPlotItem *plot)
{
    d_ptr = new QGraphicsZoomerPrivate();
    d_ptr->plot = plot;
    d_ptr->inZoom = false;
}

void QGraphicsZoomer::addScale(ScaleItem *scale)
{
    scale->installAxisChangeListener(this);
    d_ptr->zoomStackMap.insert(scale, QList<QPointF>() );
}

void QGraphicsZoomer::removeScale(ScaleItem *scale)
{
    scale->removeAxisChangeListener(this);
    d_ptr->zoomStackMap.remove(scale);
}

/** \brief Returns ScaleItem::xBottom if set
 */
ScaleItem *QGraphicsZoomer::xAxis() const
{
    foreach(ScaleItem *s, d_ptr->zoomStackMap.keys())
        if(s->axisId() == ScaleItem::xBottom)
            return s;
    return NULL;
}

ScaleItem *QGraphicsZoomer::yAxis() const
{
    foreach(ScaleItem *s, d_ptr->zoomStackMap.keys())
        if(s->axisId() == ScaleItem::yLeft)
            return s;
    return NULL;
}

void QGraphicsZoomer::plotAreaChanged(const QRectF &)
{
    /// d_ptr->zoomStack[0] = newRect;
}

int QGraphicsZoomer::stackSize() const
{
    if(d_ptr->zoomStackMap.size() > 0)
        return d_ptr->zoomStackMap.values().first().size();
    return 0;
}

void QGraphicsZoomer::unzoom()
{
    if(d_ptr->inZoom && stackSize() > 0)
    {
        foreach(ScaleItem *si, d_ptr->zoomStackMap.keys())
        {
            QList<QPointF> &zoomIntervals = d_ptr->zoomStackMap[si];
   //         qDebug() << "zoom out from " << oldRect << "to " << rect;
            if(zoomIntervals.size() > 1)
                zoomIntervals.removeLast();
            else if(zoomIntervals.size() == 1)
                d_ptr->inZoom = false;
            if(zoomIntervals.size() > 0)
            {
                si->setBounds(zoomIntervals.last().x(), zoomIntervals.last().y());
            }
        }
    }
}

void QGraphicsZoomer::clear()
{
    while(d_ptr->inZoom)
        unzoom();
}

/** \brief zoom the axes according to the zoomRect given in view coordinates
 *
 * @param zoomRect rectangle in view coordinates
 */
void QGraphicsZoomer::zoom(const QRectF& zoomRect)
{
    QPointF mP1 = zoomRect.topLeft();
    QPointF mP2 = zoomRect.bottomRight();
    double lb, ub; /* lower and upper bounds */

    if(!d_ptr->inZoom) /* reinitialize zoom */
    {
        foreach(ScaleItem *si, d_ptr->zoomStackMap.keys())
        {
            QList<QPointF> &zoomIntervals = d_ptr->zoomStackMap[si];
            QPointF initialInterval = QPointF(si->lowerBound(), si->upperBound());
            zoomIntervals.append(initialInterval);
            qDebug() << __FUNCTION__ << "initialized axis " << si->axisId() << " to axes bounds " << initialInterval;
        }
    }
    /* zoom axes. NOTE: with QMap, the items are always sorted bt key */
    foreach(ScaleItem *si, d_ptr->zoomStackMap.keys())
    {
        if(si->orientation() == ScaleItem::Horizontal)
        {
            lb = d_ptr->plot->invTransform(mP1.x(), si);
            ub = d_ptr->plot->invTransform(mP2.x(), si);
        }
        else
        {
            lb = d_ptr->plot->invTransform(mP1.y(), si);
            ub = d_ptr->plot->invTransform(mP2.y(), si);
        }
        /* set zoomed bounds on axis */
        si->setBounds(lb, ub);

        QList<QPointF> &zoomIntervals = d_ptr->zoomStackMap[si];
        QPointF interval = QPointF(si->lowerBound(), si->upperBound());
        zoomIntervals.append(interval);
    }

    //d_ptr->zoomStackMap
    qDebug() << "zoom in" << zoomRect;
    d_ptr->inZoom = true;
}

bool QGraphicsZoomer::inZoom() const
{
    return d_ptr->inZoom;
}


