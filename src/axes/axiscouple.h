#ifndef AXISCOUPLE_H
#define AXISCOUPLE_H

class ScaleItem;

#include <QPointF>

class AxisCouple
{
public:
    AxisCouple(ScaleItem *xAxis, ScaleItem *yAxis, double xOriginPosPercentage,
               double yOriginPosPercentage);

    AxisCouple(); /* builds and invalid axis couple */

    AxisCouple(const AxisCouple& other);

    bool isValid() const;

    ScaleItem* xAxis, *yAxis;

    double xOriginPosPercentage, yOriginPosPercentage;

    bool operator==(const AxisCouple &other) const;

    AxisCouple & operator=(const AxisCouple &other);
};

#endif // AXISCOUPLE_H
