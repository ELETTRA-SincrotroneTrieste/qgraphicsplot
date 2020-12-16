#include "axiscouple.h"

AxisCouple::AxisCouple(ScaleItem *xA,  ScaleItem *yA, double xOPosPercentage, double yOPosPercentage)
{
    xAxis = xA;
    yAxis = yA;
    xOriginPosPercentage = xOPosPercentage;
    yOriginPosPercentage =yOPosPercentage;
}

AxisCouple::AxisCouple()
{
    xAxis = NULL;
    yAxis = NULL;
    xOriginPosPercentage = yOriginPosPercentage = 0.5;
}

AxisCouple::AxisCouple(const AxisCouple &other) {
    xAxis = other.xAxis;
    yAxis = other.yAxis;
    xOriginPosPercentage = other.xOriginPosPercentage;
    yOriginPosPercentage = other.yOriginPosPercentage;
}

/** \brief Returns true if the axis couple is valid, false otherwise
 *
 * An axis couple is valid when at least one of the two axis is not null
 */
bool AxisCouple::isValid() const
{
    return (xAxis != NULL || yAxis != NULL );
}

bool AxisCouple::operator==(const AxisCouple &other) const
{
    return this->xAxis == other.xAxis && this->yAxis == other.yAxis;
}

AxisCouple &AxisCouple::operator=(const AxisCouple &other)
{
    if(this == &other)
        return *this;
    this->xAxis = other.xAxis;
    this->yAxis = other.yAxis;
    this->xOriginPosPercentage = other.xOriginPosPercentage;
    this->yOriginPosPercentage = other.yOriginPosPercentage;
    return *this;
}
