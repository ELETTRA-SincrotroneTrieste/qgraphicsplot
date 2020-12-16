#ifndef HISTOGRAMPAINTERPRIVATE_H
#define HISTOGRAMPAINTERPRIVATE_H

#include <QPen>
#include <QBrush>
#include <QHash>

class CurveItem;

class HistogramPainterPrivate
{
public:
    HistogramPainterPrivate();

    CurveItem* curveItem;

    QPen pen;

    QBrush brush;

    double width, baseline;

    bool autoWidth;

    QHash<int, QColor> colorHash;
};

#endif // HISTOGRAMPAINTERPRIVATE_H
