#ifndef CURVESMAP_H
#define CURVESMAP_H

#include <QString>
#include <QMap>

class SceneCurve;

class CurveAssoc
{
public:
    CurveAssoc();

    CurveAssoc(const QString& na, int aId, SceneCurve *c);

    QString name;

    int axisId;

    SceneCurve *curve;
};

class CurvesMap
{
public:

    CurvesMap();

    void remove(const QString& name);

    void clear();

    void addCurve(const QString& na,  SceneCurve *c);

    int getNewAxisId() const;

    bool hasCurve(const QString& name) const;

    int getAxisId(const QString& name) const;

    SceneCurve *getCurve(const QString& name) const;

    int size() const;

private:
    QMap<QString, CurveAssoc> mMap;

    int mLastAxisId;
};

#endif // CURVESMAP_H
