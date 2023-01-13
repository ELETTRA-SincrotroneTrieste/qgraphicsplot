#ifndef QGRAPHICSZOOMER_H
#define QGRAPHICSZOOMER_H

#include <axischangelistener.h>

class QGraphicsPlotItem;
class QGraphicsZoomerPrivate;
class QRectF;
class ScaleItem;

class QGraphicsZoomer :  public AxisChangeListener
{
public:
    explicit QGraphicsZoomer(QGraphicsPlotItem *plot);

    void addScale(ScaleItem *scale);

    void removeScale(ScaleItem *scale);

    virtual ~QGraphicsZoomer();

    void zoom(const QRectF& zoomRect);

    bool inZoom() const;

    void unzoom();

    void clear();

    int stackSize() const;

    void canvasRectChanged(const QRectF &newRect);

    void xAxisBoundsChanged(double , double ) {}

    void yAxisBoundsChanged(double , double ) {}

    void axisAutoscaleChanged(ScaleItem::Orientation , bool ) {}

    virtual void tickStepLenChanged(double  ) {}

    virtual void labelsFormatChanged(const QString& ) {}

    ScaleItem *xAxis() const;

    ScaleItem *yAxis() const;

private:

    QGraphicsZoomerPrivate *d_ptr;
};

#endif // ZOOMER_H
