#include "histogrampainter.h"
#include "../../colors.h"
#include "../scenecurve.h"
#include "../curveitem.h"
#include "histogrampainterprivate.h"
#include <QPainter>
#include <QtDebug>

HistogramPainter::HistogramPainter(CurveItem* curveItem) : QObject(curveItem)
{
    d_ptr = new HistogramPainterPrivate();
    d_ptr->curveItem = curveItem;
    d_ptr->pen = QPen(KBLUE);
    d_ptr->brush = QBrush(KDARKBLUE);
    d_ptr->baseline = 0.0;
    d_ptr->width = 5;
    d_ptr->autoWidth = true;
    curveItem->installItemPainterInterface(this);
    setObjectName("HistogramPainter");
}

void HistogramPainter::draw(SceneCurve *curve,
                      QPainter *painter,
                      const QStyleOptionGraphicsItem * option,
                      QWidget * widget)
{
    printf("%s \e[1;31m not implemented\e[0m\n", __PRETTY_FUNCTION__);
//    Q_UNUSED(widget);
//    Q_UNUSED(option);
//    int dataSiz = curve->dataSize();
//    if(dataSiz < 2)
//        return;

// //   painter->save();
//    const QPointF *points = curve->points();
//    double x, y, yBaseLine;
//    double width;
//    if(d_ptr->autoWidth)
//        width = curve->getXAxis()->canvasWidth / (curve->dataSize() * 1.8);
//    else
//        width = d_ptr->width;

//    if(d_ptr->colorHash.isEmpty())
//    {
//        painter->setBrush(d_ptr->brush);
//        painter->setPen(d_ptr->pen);
//    }

//    for(int i = 0; i < curve->dataSize(); i++)
//    {
//        x = points[i].x() - width/2.0;
//        y = points[i].y();

//        if(d_ptr->colorHash.contains(i))
//        {
//            painter->setPen(d_ptr->colorHash.value(i).darker());
//            painter->setBrush(d_ptr->colorHash.value(i));
//        }

//        yBaseLine = plot->transform(d_ptr->baseline, plot->yScaleItem());
////        qDebug() << __FUNCTION__ << "yBaseline " << yBaseLine << "d_ptr->baseline" << d_ptr->baseline << "y"
////                 << y << curve->getXAxis()->canvasWidth << curve->getXAxis()->canvasHeight;

//        double x1 = plot->transform(plot->xScaleItem()->lowerBound(), plot->xScaleItem());
//        double x2 = plot->transform(plot->xScaleItem()->upperBound(), plot->xScaleItem());



//        QColor rc = d_ptr->pen.color();
//        if(y > yBaseLine)
//        {
//            painter->setPen(rc);
//            painter->drawRect(x, yBaseLine, width, y - yBaseLine);
//        }
//        else
//        {
//            painter->setPen(rc);
//            painter->drawRect(x, y, width, yBaseLine - y);
//        }

//        /// TEST ///
//        /// painter->setPen(Qt::red);
//        /// painter->drawLine(x1, yBaseLine, x2, yBaseLine);
//    }
//  //  painter->restore();
}

CurveItem* HistogramPainter::curveItem() const
{
    return d_ptr->curveItem;
}

int HistogramPainter::type() const
{
    return ItemPainterInterface::Histogram;
}

QSizeF HistogramPainter::elementSize() const
{
    return QSizeF(0, 0);
}

QColor HistogramPainter::color() const
{
    return d_ptr->brush.color();
}

double HistogramPainter::width() const
{
    return d_ptr->width;
}

double HistogramPainter::baseLine() const
{
    return d_ptr->baseline;
}

void HistogramPainter::setWidth(double w)
{
    d_ptr->width = w;
}

void HistogramPainter::setBaseLine(double baseline)
{
    d_ptr->baseline = baseline;
}

void HistogramPainter::setColor(const QColor& c)
{
    d_ptr->pen.setColor(c.darker());
    d_ptr->brush.setColor(c);
}

bool HistogramPainter::autoWidth() const
{
    return d_ptr->autoWidth;
}

void HistogramPainter::setAutoWidth(bool aw)
{
    d_ptr->autoWidth = aw;
}

void  HistogramPainter::setColorAt(int index, const QColor &c)
{
    d_ptr->colorHash.insert(index, c);
}

QColor HistogramPainter::colorAt(int index) const
{
    return d_ptr->colorHash.value(index);
}

/** \brief Implements the pen method of the ItemPainterInterface
 *
 * @return the pen used to draw the rects
 */
QPen HistogramPainter::pen() const
{
    return d_ptr->pen;
}





