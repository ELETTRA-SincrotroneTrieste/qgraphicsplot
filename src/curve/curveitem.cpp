#include "curveitem.h"
#include "curveitemprivate.h"
#include "../qgraphicsplotmacros.h"
#include "scenecurve.h"
#include "point.h"
#include "data.h"
#include "pointdata.h"
#include "itempainterinterface.h"
#include <QtDebug>
#include <QStyleOptionGraphicsItem>

/** \brief The constructor of this QGraphicsObject that connects the elements
           of the associated SceneCurve by means of lines.
  *
  * @param curve the associated SceneCurve
  *
  * @see SceneCurve
  */
CurveItem::CurveItem(SceneCurve *curve) : QGraphicsObject(), CurveChangeListener()
{
    d_ptr = new CurveItemPrivate();
    d_ptr->curve = curve;
    d_ptr->curve->setCurveItem(this);
    /* needed for configurable properties */
    setObjectName(curve->objectName() + " Item");
}

CurveItem::~CurveItem()
{
    qDebug() << __FUNCTION__ << "deleting curve item " << this->objectName();
    foreach(ItemPainterInterface *p, d_ptr->itemPainters)
        delete p;
    d_ptr->itemPainters.clear();
   delete d_ptr;
   d_ptr = NULL;
   qDebug() << __FUNCTION__ << "exit";
}

SceneCurve *CurveItem::sceneCurve() const
{
    return d_ptr->curve;
}

/** \brief If <em>only one ItemPainterInterface</em> was used, then use this
 *  method to retrieve a reference to it.
 */
ItemPainterInterface *CurveItem::itemPainter() const
{
    if(d_ptr->itemPainters.size() > 0)
        return d_ptr->itemPainters.last();
    return NULL;
}

QList<ItemPainterInterface *> CurveItem::itemPainters() const
{
    return d_ptr->itemPainters;
}

void CurveItem::installItemPainterInterface(ItemPainterInterface *itemPainterInterface)
{
    d_ptr->itemPainters.append(itemPainterInterface);
}

void CurveItem::removeItemPainterInterface(ItemPainterInterface *itemPainterInterface)
{
    if(d_ptr->itemPainters.contains(itemPainterInterface))
        d_ptr->itemPainters.removeAll(itemPainterInterface);
}

/* we are ItemChangeListeners, itemAdded is invoked after  SceneCurve::addPoint(XYItem *item)
 * when the item position is determined, minimum and maximum bounds have been updated
 * the buffer size has been checked and the item is added to the scene.
 */
void CurveItem::itemAdded(const Point &)
{

    if(!isVisible())
        setVisible(true);

    Data *data = d_ptr->curve->data();
    //    /* recalculate the bounding rect only if bounds have changed */

    //    /* update region */
    int itemCnt = data->size();

    if(itemCnt < 2)
        return;

//    update();
//    return;
    ScaleItem *xScale = d_ptr->curve->getXAxis();
    ScaleItem *yScale =d_ptr->curve->getYAxis();

    if(data->xData.last() < xScale->upperBound() &&
            data->yData.last() < yScale->upperBound() &&
            data->yData.last() > yScale->lowerBound())
    {
        double x1, x2, y1, y2;

        /* item painters may need extra space to take into account */
        double extraX = 0.0, extraY = 0.0;
        foreach(ItemPainterInterface* i, d_ptr->itemPainters)
        {
            if(i->elementSize().width() > extraX)
                extraX = i->elementSize().width();
            if(i->elementSize().height() > extraY)
                extraY = i->elementSize().height();
        }

        x1 = d_ptr->curve->plot()->transform(data->xData[itemCnt - 2], xScale) - extraX;
        y1 = d_ptr->curve->plot()->transform(data->yData[itemCnt - 2], yScale) - extraY;
        x2 = d_ptr->curve->plot()->transform(data->xData[itemCnt - 1], xScale) + extraX;
        y2 = d_ptr->curve->plot()->transform(data->yData[itemCnt - 1], yScale) + extraY;
        QPointF topLeft(qMin(x1, x2), qMin(y1, y2));
        QPointF botRight(qMax(x1, x2), qMax(y1, y2));
        QRectF updateRect(topLeft, botRight);
        d_ptr->updateRect = updateRect;
        update(updateRect);

//        qDebug() << __FUNCTION__ << ":-) partial update possible";
    }
    else if(itemCnt > 1)
    {
//        qDebug() << __FUNCTION__ << ":-( partial update NOT possible";
        update();
    }
}

void CurveItem::itemAboutToBeRemoved(const Point &)
{
    if(d_ptr->curve->dataSize() == 0)
        setVisible(false);

//   // update();
//    return;

    Data *data = d_ptr->curve->data();
    int itemCnt = data->size();
    if(itemCnt > 1)
    {
        double x1, x2, y1, y2;

        double extraX = 0.0, extraY = 0.0;
        /* item painters may need extra space to take into account */
        foreach(ItemPainterInterface* i, d_ptr->itemPainters)
        {
            if(i->elementSize().width() > extraX)
                extraX = i->elementSize().width();
            if(i->elementSize().height() > extraY)
                extraY = i->elementSize().height();
        }
        x1 = d_ptr->curve->plot()->transform(data->xData[0], d_ptr->curve->getXAxis()) - extraX;
        y1 = d_ptr->curve->plot()->transform(data->yData[0], d_ptr->curve->getYAxis()) - extraY;
        x2 = d_ptr->curve->plot()->transform(data->xData[1], d_ptr->curve->getXAxis()) + extraX;
        y2 = d_ptr->curve->plot()->transform(data->yData[1], d_ptr->curve->getYAxis()) + extraY;

        QPointF topLeft(qMin(x1, x2), qMin(y1, y2));
        QPointF botRight(qMax(x1, x2), qMax(y1, y2));
        QRectF updateRect(topLeft, botRight);
        d_ptr->updateRectDelete = updateRect;
//        qDebug() << __FUNCTION__ << " updating a rect in REMOVAL " << updateRect;
        // update(updateRect);
    }
}

void CurveItem::fullVectorUpdate()
{
    update();
}

QRectF CurveItem::boundingRect() const
{
    QRectF br;
    br = scene()->sceneRect();
    return br;
}

void CurveItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    printf("\e[1;34mCurveItem %s: paint zValue %f\e[0m\n", qstoc(objectName()), zValue());

    /* setting clipRegion freezes app when using Open-GL */
    /* setting a clip on the painter saves PlotSceneWidget's LIFE!!
     * Great comments on
     * http://thesmithfam.org/blog/2007/02/03/qt-improving-qgraphicsview-performance/comment-page-1
     */
    painter->setClipRect(option->exposedRect.toRect());

    if(d_ptr->itemPainters.size())
    {
        foreach(ItemPainterInterface* ipi, d_ptr->itemPainters)
            ipi->draw(d_ptr->curve, d_ptr->curve->plot(), painter, option, widget);
    }
    else
        perr("CurveItem::paint(): no item painters installed!");

    #ifdef DEBUG_PAINT

    {
        painter->setPen(Qt::green);
        painter->drawRect(boundingRect());
            painter->setPen(Qt::darkBlue);
            painter->drawEllipse(boundingRect().topLeft(), 5, 5);
            painter->setPen(Qt::red);
            painter->drawRect(option->exposedRect.toRect());
    }


//    painter->setPen(Qt::red);
//    painter->drawRect(d_ptr->updateRectDelete);
    #endif
}




