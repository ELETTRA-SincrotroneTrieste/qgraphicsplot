#include "stepspainter.h"
#include "stepspainterprivate.h"
#include <curveitem.h>
#include <plotscenewidget.h>
#include "../scenecurve.h"
#include "../curveitem.h"
#include "../data.h"
#include <QtDebug>
#include <QStyleOptionGraphicsItem>

StepsPainter::StepsPainter(CurveItem *curveItem) :  QObject(curveItem)
{
    d_ptr = new StepsPainterPrivate();
    d_ptr->curveItem = curveItem;
    d_ptr->pen.setWidthF(0.0);
    curveItem->installItemPainterInterface(this);
    setObjectName("StepsPainter");
}

StepsPainter::~StepsPainter()
{
    qDebug() << __FUNCTION__ << "deleting " << objectName() << "removing painter from curve item "
             << d_ptr->curveItem->objectName();
    delete d_ptr;
}

void StepsPainter::draw(SceneCurve *curve,
                  PlotSceneWidget* plot,
                  QPainter *painter,
                  const QStyleOptionGraphicsItem * ,
                  QWidget * )
{
    Q_UNUSED(plot);
    int dataSiz = curve->dataSize();
    painter->setPen(d_ptr->pen);
    const QPointF *points = curve->points();
    painter->setBrush(QBrush(d_ptr->pen.color()));
    if(dataSiz == 1)
        painter->drawEllipse(points[0], 3, 2.5);
    else
    {
        for(int i = 0; i < dataSiz - 1; i++)
        {
            painter->drawLine(points[i].x(), points[i].y(), points[i + 1].x(), points[i].y());
            painter->drawLine(points[i + 1].x(), points[i].y(), points[i + 1].x(), points[i + 1].y());
        }
    }
    /* draw NaNs (invalid data */
    QVector<double> xInvalid = curve->data()->invalidDataPoints();
    if(xInvalid.size() > 0)
    {
        painter->setPen(Qt::red);
        foreach(double d, xInvalid)
        {
            painter->drawLine(plot->transform(d, plot->xScaleItem()), 0,
                              plot->transform(d, plot->xScaleItem()),
                              painter->clipBoundingRect().height());
        }
        painter->setPen(d_ptr->pen);
    }
}

int StepsPainter::type() const
{
    return ItemPainterInterface::Step;
}

QSizeF StepsPainter::elementSize() const
{
    return QSizeF(d_ptr->pen.widthF(), d_ptr->pen.widthF());
}

CurveItem *StepsPainter::curveItem() const
{
    return d_ptr->curveItem;
}

QColor  StepsPainter::lineColor() const
{
    return d_ptr->pen.color();
}

double StepsPainter::lineWidth() const
{
    return d_ptr->pen.widthF();
}

QPen StepsPainter::linePen() const
{
    return d_ptr->pen;
}

/** \brief Implements ItemPainterInterface::pen method
 *
 * @return the line pen as returned by linePen
 */
QPen StepsPainter::pen() const
{
    return d_ptr->pen;
}


void StepsPainter::setLineColor(const QColor& c)

{
    d_ptr->pen.setColor(c);
    d_ptr->curveItem->update();
}

void StepsPainter::setLineWidth(double w)
{
    d_ptr->pen.setWidthF(w);
    d_ptr->curveItem->update();
}

void StepsPainter::setLinePen(const QPen& p)
{
    d_ptr->pen = p;
    d_ptr->curveItem->update();
}

