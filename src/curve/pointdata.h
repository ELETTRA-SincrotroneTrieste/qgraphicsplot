#ifndef POINTDATA_H
#define POINTDATA_H

#include <time.h>
#include <QtAlgorithms> /* qreal */
#include <QMetaType>

class PointData
{
public:
    PointData(double _x, double _y, time_t ts = 0, qreal = 0);
    PointData();

    double x;
    double y;

    qreal order;

    time_t timestamp;

    bool valid;
};

Q_DECLARE_METATYPE(PointData)

#endif // POINTDATA_H
