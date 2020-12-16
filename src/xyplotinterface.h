#ifndef XYPLOTINTERFACE_H
#define XYPLOTINTERFACE_H

class PointData;
class Point;

class XYPlotInterface
{
public:
    XYPlotInterface() {}

    virtual void appendData(const QString& curveName, double x, double y) = 0;

    virtual void setData(const QString& curveName,
                         const QVector< double > &xData,
                         const QVector< double > &yData) = 0;

    virtual void setData(const QString& curveName,
                         const QVector< double > &yData) = 0;

};

#endif // XYPLOTINTERFACE_H
