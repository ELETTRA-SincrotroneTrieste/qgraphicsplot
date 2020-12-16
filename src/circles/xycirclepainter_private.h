#ifndef XYCIRCLEPAINTER_PRIVATE_H
#define XYCIRCLEPAINTER_PRIVATE_H

#include <QColor>

class Point;


class XYCirclePainterPrivate
{
public:
    XYCirclePainterPrivate();

    qreal radius, maxRadius;

    Point *item;

    QColor color, borderColor, gradientStopColor;

    bool gradientEnabled, borderEnabled;
};

#endif // XYCIRCLEPAINTER_PRIVATE_H
