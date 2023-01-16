#ifndef CURVECHANGELISTENER_H
#define CURVECHANGELISTENER_H

class Point;
class QRectF;
class SceneCurve;

class CurveChangeListener
{
public:
    CurveChangeListener() {}

    virtual QRectF itemAdded(const Point &pt) = 0;

    virtual void itemAboutToBeRemoved(const Point &pt) = 0;

    virtual QRectF itemRemoved(const Point &pt) = 0;

    virtual void fullVectorUpdate() = 0;

    virtual void bufferSizeChanged(int newSize) = 0;

    virtual void minXChanged(double min) = 0;

    virtual void maxXChanged(double max) = 0;

    virtual void minYChanged(double  min) = 0;

    virtual void maxYChanged(double max) = 0;

    virtual void affectingBoundsPointsRemoved() = 0;
};

#endif // ITEMCHANGELISTENER_H
