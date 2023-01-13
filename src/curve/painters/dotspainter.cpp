#include "dotspainter.h"
#include "../../colors.h"
#include "../scenecurve.h"
#include "../curveitem.h"
#include "dotspainterprivate.h"
#include <QPainter>

DotsPainter::DotsPainter(CurveItem *curveItem) : QObject(curveItem)
{
    d_ptr = new DotsPainterPrivate();
    d_ptr->curveItem = curveItem;
    d_ptr->brush = QBrush(KYELLOW);
    d_ptr->pen = QPen(KMAROON);
    d_ptr->pen.setWidthF(0.0);
    d_ptr->radius = 1.0;
    curveItem->installItemPainterInterface(this);
    setObjectName("DotsPainter");
}

DotsPainter::~DotsPainter()
{
    d_ptr->curveItem->removeItemPainterInterface(this);
    delete d_ptr;
}

void DotsPainter::draw(SceneCurve *curve,
                  QPainter *painter,
                  const QStyleOptionGraphicsItem * ,
                  QWidget * )
{
    int dataSiz = curve->dataSize();
    if(dataSiz < 2)
        return;

    painter->save();
    painter->setPen(d_ptr->pen);
    const QPointF *points = curve->points();
    for(int i = 0; i < curve->dataSize(); i++)
    {
        painter->setBrush(d_ptr->pen.color());
        painter->setPen(d_ptr->pen);
        QPointF p = points[i];
     //   painter->fillRect(QRectF(p.x() - d_ptr->radius, p.y() - d_ptr->radius,
       //                   p.x() + d_ptr->radius, p.y() + d_ptr->radius), d_ptr->pen.color());
      painter->drawEllipse(p, d_ptr->radius, d_ptr->radius);
    }
    /* draw NaNs (invalid data) */
    QVector<double> xInvalid = curve->data()->invalidDataPoints();
    if(xInvalid.size() > 0)
    {
        painter->setPen(Qt::red);
        foreach(double d, xInvalid)
        {
            printf("%s \e[1;31m check draw invalid points\e[0m\n", __PRETTY_FUNCTION__);
//            painter->drawLine(plot->transform(d, plot->xScaleItem()), 0,
//                              plot->transform(d, plot->xScaleItem()),
//                              painter->clipBoundingRect().height());
        }
        painter->setPen(d_ptr->pen);
    }

    painter->restore();
}

int DotsPainter::type() const
{
    return ItemPainterInterface::Dot;
}

QSizeF DotsPainter::elementSize() const
{
    return QSizeF(d_ptr->pen.widthF(), d_ptr->pen.widthF());
}

CurveItem *DotsPainter::curveItem() const
{
    return d_ptr->curveItem;
}

QColor DotsPainter::borderColor() const
{
    return d_ptr->pen.color();
}

QColor DotsPainter::dotsColor() const
{
    return d_ptr->brush.color();
}

double DotsPainter::radius() const
{
    return d_ptr->pen.widthF();
}

/** \brief Implements the pen method of the ItemPainterInterface
 *
 * @return the pen used to draw the circles
 */
QPen DotsPainter::pen() const
{
    return d_ptr->pen;
}

void DotsPainter::setDotsColor(const QColor &c)
{
    d_ptr->brush.setColor(c);
    d_ptr->curveItem->update();
}

void DotsPainter::setBorderColor(const QColor& c)

{
    d_ptr->pen.setColor(c);
    d_ptr->curveItem->update();
}

void DotsPainter::setRadius(double w)
{
    d_ptr->pen.setWidthF(w);
    d_ptr->curveItem->update();
}

void DotsPainter::setPen(const QPen& p)
{
    d_ptr->pen = p;
    d_ptr->curveItem->update();
}

