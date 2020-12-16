#ifndef STEPSPAINTERPRIVATE_H
#define STEPSPAINTERPRIVATE_H

#include <QPen>

class CurveItem;

class StepsPainterPrivate
{
public:
    StepsPainterPrivate();

    QPen pen;

    CurveItem* curveItem;
};

#endif // STEPSPAINTERPRIVATE_H
