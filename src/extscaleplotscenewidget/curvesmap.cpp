#include "curvesmap.h"

CurveAssoc::CurveAssoc()
{
    axisId = -1;
    curve = NULL;
}

CurveAssoc::CurveAssoc(const QString &na, int aId, SceneCurve *c)
{
    name = na;
    axisId = aId;
    curve = c;
}

CurvesMap::CurvesMap()
{
    mLastAxisId = 100;
}

void CurvesMap::remove(const QString &name)
{
    mMap.remove(name);
}

void CurvesMap::clear()
{
    mMap.clear();
}

void CurvesMap::addCurve(const QString &na, SceneCurve *c)
{
    CurveAssoc ca(na, ++mLastAxisId, c);
    mMap.insert(na, ca);
}

int CurvesMap::getNewAxisId() const
{
    return mLastAxisId;
}

bool CurvesMap::hasCurve(const QString &name) const
{
    return mMap.contains(name);
}

int CurvesMap::getAxisId(const QString &name) const
{
    if(mMap.contains(name))
        return mMap[name].axisId;
    return -1;
}

SceneCurve *CurvesMap::getCurve(const QString &name) const
{
    SceneCurve *c = NULL;
    if(mMap.contains(name))
        c = mMap[name].curve;
    return c;
}

int CurvesMap::size() const
{
    return mMap.size();
}
