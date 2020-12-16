#ifndef XYCIRCLEPAINTER_H
#define XYCIRCLEPAINTER_H

#include "itempainterinterface.h"
#include <QRectF>
#include <QColor>

class XYCirclePainterPrivate;

class XYCirclePainter : public ItemPainterInterface
{
public:
    XYCirclePainter(Point *item);

    virtual void draw(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QRectF boundingRect() const;

    void setRadius(qreal r);

    void setColor(const QColor& c);

    QColor color() const;

    qreal radius() const;

    QColor borderColor() const;

    void setBorderEnabled(bool en);

    bool borderEnabled() const;

    void setBorderColor(const QColor& c);

    void setGradientEnabled(bool en);

    bool gradientEnabled() const;

    void setGradientStopColor(const QColor& c);

    QColor gradientStopColor() const;

private:
    XYCirclePainterPrivate* d_ptr;

    Q_DECLARE_PRIVATE(XYCirclePainter)
};

#endif // XYCIRCLEPAINTER_H
