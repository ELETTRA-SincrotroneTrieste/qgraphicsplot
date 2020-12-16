#ifndef DATA_H
#define DATA_H

#include <QVector>
#include <QPointF>
#include "point.h"

class SceneCurve;
class QRectF;

class Data
{
public:
    Data();


    virtual ~Data();

    double xMin, xMax, yMin, yMax;

    bool xMinMaxUnset , yMinMaxUnset;

    bool scalarMode;

    bool xDataOrdered, yDataOrdered;

    bool dataUnchanged() const;

    void cacheData();

    QVector<double> xData;
    QVector<double> yData;

    Point point(int index) const;

    double xpos(int index);

    double ypos(int index);

    void remove(int index);

    void calculateXBounds();

    void calculateYBounds();

    void calculateBounds();

    /** \brief sets the x data valid so that subsequent calls can use cached values
      *        for the position in scene coordinates.
      *
      * @param valid true the cached data is marked as valid
      * @param valid false the cached data is marked as invalid
      *
      * If data is marked as valid xpos will return the cached value of the
      * position in scene coordinates. Otherwise, it will calculate it again
      * using the plot rect, the axis upper and lower bound and the index of
      * the data whose position has to be calculated.
      */
    void markLastXDataValid(int lastValidPos) { lastValidXPos = lastValidPos; }


    /** \brief sets the y data valid so that subsequent calls can use cached values
      *        for the position in scene coordinates.
      *
      * @param valid true the cached data is marked as valid
      * @param valid false the cached data is marked as invalid
      *
      * If data is marked as valid ypos will return the cached value of the
      * position in scene coordinates. Otherwise, it will calculate it again
      * using the plot rect, the axis upper and lower bound and the index of
      * the data whose position has to be calculated.
      */
    void markLastYDataValid(bool lastValidPos) { lastValidYPos = lastValidPos; }

    int size() const;

    void setData(const QVector<double> &xData, const QVector<double> &yData);

    void setData(const QVector<double> &yData);

    void addPoint(double x, double y);

    void addPoints(const QVector<double> &xData, const QVector<double> &yData);

    QVector<double> invalidDataPoints() const;

    void resetMaxMin();

private:

    int lastValidXPos, lastValidYPos;


    bool mXDataChanged, mYDataChanged;
    bool dataChanged;

};

#endif // DATA_H
