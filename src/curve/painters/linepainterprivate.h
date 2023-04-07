#ifndef LINEPAINTERPRIVATE_H
#define LINEPAINTERPRIVATE_H

#include <QPen>
#include <QBrush>

class CurveItem;

class LinePainterPrivate
{
public:
    LinePainterPrivate();

    QPen pen;
    QBrush brush;

    CurveItem* curveItem;
};

#endif // LINEPAINTERPRIVATE_H
