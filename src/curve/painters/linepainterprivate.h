#ifndef LINEPAINTERPRIVATE_H
#define LINEPAINTERPRIVATE_H

#include <QPen>

class CurveItem;

class LinePainterPrivate
{
public:
    LinePainterPrivate();

    QPen pen;

    CurveItem* curveItem;
};

#endif // LINEPAINTERPRIVATE_H
