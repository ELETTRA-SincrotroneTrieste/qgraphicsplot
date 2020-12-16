#include "xycirclepainter.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QRectF>
#include <QRadialGradient>
#include "xycirclepainter_private.h"
#include "curve/point.h"
#include "colors.h"
#include <stdio.h>
#include <QtDebug>

XYCirclePainter::XYCirclePainter(Point* item)
{
    d_ptr = new XYCirclePainterPrivate();
    d_ptr->radius = d_ptr->maxRadius = 5;
    d_ptr->item = item;
    d_ptr->borderColor = KDARKGRAY;
    d_ptr->color = KGRAY;
    d_ptr->gradientEnabled = false;
    d_ptr->gradientStopColor = Qt::white;
    d_ptr->borderEnabled = true;
}

void XYCirclePainter::draw(QPainter *painter,
                           const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    //    printf("drawing XYCirclePainter radius %f\n", d_ptr->radius);


    QPen p;

    if(d_ptr->borderEnabled)
        p.setColor(d_ptr->color.darker());
    else
        p.setColor(d_ptr->color);
    painter->setPen(p);


    if(d_ptr->gradientEnabled)
    {
        QRadialGradient rg(QPointF(0,0), d_ptr->radius);
        rg.setColorAt(0, d_ptr->color.lighter());
        rg.setColorAt(1, d_ptr->color);
        painter->setBrush(rg);
    }
    else
        painter->setBrush(QBrush(d_ptr->color));

    painter->drawEllipse(-d_ptr->radius/2.0, -d_ptr->radius/2.0, d_ptr->radius, d_ptr->radius);
}

QRectF XYCirclePainter::boundingRect() const
{
    return QRectF(-d_ptr->maxRadius/2.0, -d_ptr->maxRadius/2.0, d_ptr->maxRadius, d_ptr->maxRadius);
}

void XYCirclePainter::setColor(const QColor& c)
{
    d_ptr->color = c;
}

QColor XYCirclePainter::color() const
{
    return d_ptr->color;
}

QColor XYCirclePainter::borderColor() const
{
    return d_ptr->borderColor;
}

void XYCirclePainter::setBorderColor(const QColor& c)
{
    d_ptr->borderColor = c;
}

void XYCirclePainter::setGradientEnabled(bool en)
{
    d_ptr->gradientEnabled = en;
}

bool XYCirclePainter::gradientEnabled() const
{
    return d_ptr->gradientEnabled;
}

void XYCirclePainter::setGradientStopColor(const QColor& c)
{
    d_ptr->gradientStopColor = c;
}

QColor XYCirclePainter::gradientStopColor() const
{
    return d_ptr->gradientStopColor;
}

void XYCirclePainter::setBorderEnabled(bool en)
{
    d_ptr->borderEnabled = en;
}

bool XYCirclePainter::borderEnabled() const
{
    return d_ptr->borderEnabled;
}


void XYCirclePainter::setRadius(qreal r)
{
    qreal updateRadius = r;
    bool doUpdate = false;
    if(d_ptr->radius != r)
        doUpdate = true;
    if(d_ptr->radius > r)
        updateRadius = d_ptr->radius;
    /* store new value */
    d_ptr->radius = r;

    if(r > d_ptr->maxRadius)
        d_ptr->maxRadius = r;

    if(doUpdate)
        d_ptr->item->update(0, 0, updateRadius, updateRadius);
}

qreal XYCirclePainter::radius() const
{
    return d_ptr->radius;
}
