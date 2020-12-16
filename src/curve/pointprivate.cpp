#include "pointprivate.h"

PointPrivate::PointPrivate(const PointData &pointData)
{
    data = pointData;
    curveId = -1;
}
