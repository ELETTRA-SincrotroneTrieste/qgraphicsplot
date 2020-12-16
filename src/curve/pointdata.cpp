#include "pointdata.h"

PointData::PointData(double _x, double _y, time_t ts, qreal)
{
    x = _x;
    y = _y;
    timestamp =  ts;
    valid = true;
}

PointData::PointData()
{
    x = 0.0;
    y = 0.0;
    valid = false;
}
