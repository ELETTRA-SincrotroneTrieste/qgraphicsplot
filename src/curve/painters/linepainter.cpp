#include "linepainter.h"
#include "../scenecurve.h"
#include "../curveitem.h"
#include "../data.h"
#include "linepainterprivate.h"
#include <QtDebug>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPainter>
#include "qgraphicsplotmacros.h"

LinePainter::LinePainter(CurveItem *curveItem) : QObject(curveItem)
{
    d_ptr = new LinePainterPrivate();
    d_ptr->curveItem = curveItem;
    d_ptr->pen.setWidthF(0.0);
    curveItem->installItemPainterInterface(this);
    setObjectName("LinePainter");
}

LinePainter::~LinePainter()
{
    qDebug() << __FUNCTION__ << "deleting " << objectName() << "removing painter from curve item "
             << d_ptr->curveItem->objectName();
   // d_ptr->curveItem->removeItemPainterInterface(this);
    delete d_ptr;
}

void LinePainter::draw(SceneCurve *curve,
                  QPainter *painter,
                  const QStyleOptionGraphicsItem * ,
                  QWidget * ) {
    qDebug() << __PRETTY_FUNCTION__ << "paint rect" << painter->viewport() << "window" << painter->window();
    QPen pen = painter->pen();
    painter->drawRect(painter->viewport());
    painter->setPen(Qt::red);
    painter->drawRect(painter->window());
    painter->setPen(pen);
    int dataSiz = curve->dataSize();
    painter->setPen(d_ptr->pen);
    const QPointF *points = curve->points();
    if(dataSiz <= 2)
    {
        painter->setBrush(QBrush(d_ptr->pen.color()));
        for(int i = 0; i < dataSiz; i++)
        {
            painter->drawEllipse(points[i], 3, 2.5);
        }
    }
    if(points)
    {
        painter->drawPolyline(points, dataSiz);
//        for(int i = 0; i < dataSiz; i++)
//            printf("\e[1;33m(%f,%f), ", points[i].x(), points[i].y());
//        printf("\e[0m\n\n");
    }
    /* draw NaNs (invalid data */
    QVector<double> xInvalid = curve->data()->invalidDataPoints();
    if(xInvalid.size() > 0)
    {
        QPen invalidDataPen(Qt::red);
        invalidDataPen.setWidthF(0.0);
        painter->setPen(invalidDataPen);
        foreach(double d, xInvalid)
        {
            printf("%s \e[1;31mShould draw invalid points!!! \e[0m\n", __PRETTY_FUNCTION__);
//            painter->drawLine(plot->transform(d, plot->scaleItem(curve->getXAxis()->axisId())), 0,
//                              plot->transform(d, plot->scaleItem(curve->getXAxis()->axisId())),
//                              painter->clipBoundingRect().height());
        }
        painter->setPen(d_ptr->pen);
    }
}

int LinePainter::type() const
{
    return ItemPainterInterface::Line;
}

QSizeF LinePainter::elementSize() const
{
    return QSizeF(d_ptr->pen.widthF(), d_ptr->pen.widthF());
}

CurveItem *LinePainter::curveItem() const
{
    return d_ptr->curveItem;
}

QColor  LinePainter::lineColor() const
{
    return d_ptr->pen.color();
}

double LinePainter::lineWidth() const
{
    return d_ptr->pen.widthF();
}

QPen LinePainter::linePen() const
{
    return d_ptr->pen;
}

QPen LinePainter::pen() const
{
    return d_ptr->pen;
}

void LinePainter::setLineColor(const QColor& c)

{
    d_ptr->pen.setColor(c);
    d_ptr->curveItem->update();
}

void LinePainter::setLineWidth(double w)
{
    d_ptr->pen.setWidthF(w);
    d_ptr->curveItem->update();
}

void LinePainter::setLinePen(const QPen& p)
{
    d_ptr->pen = p;
    d_ptr->curveItem->update();
}
