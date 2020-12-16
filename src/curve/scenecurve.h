#ifndef SCENECURVE_H
#define SCENECURVE_H

#include <QObject>
#include <plotscenewidget.h>
#include <axischangelistener.h>
#include <data.h>

class SceneCurvePrivate;
class QGraphicsScene;
class ScaleItem;
class CurveChangeListener;
class CurveItem;


class SceneCurve : public QObject, public AxisChangeListener
{
    Q_OBJECT

    Q_PROPERTY(int bufferSize READ bufferSize WRITE setBufferSize)
    Q_PROPERTY(bool xDataIsOrdered READ xDataIsOrdered WRITE setXDataIsOrdered)
    Q_PROPERTY(bool yDataIsOrdered READ yDataIsOrdered WRITE setYDataIsOrdered)

public:

    explicit SceneCurve(PlotSceneWidget *sceneWidget,
                        const QString& name,
                        ScaleItem* xAxis,
                        ScaleItem* yAxis);

    virtual ~SceneCurve();

    /** \brief returns a reference to the PlotSceneWidget to which this curve
      *        has been attached.
      *
      */
    PlotSceneWidget* plot() const;

    /** \brief returns the QGraphicsScene used by the PlotSceneWidget.
      *
      * It internally calls plot()->scene();
      */
    QGraphicsScene *scene() const;

    /** \brief returns the current number of XYItems (points) in the curve.
      *
      */
    int dataSize() const;

    /** \brief returns the curve buffer size
      *
      * @return the number of maximum items allowed in the curve.
      *
      * When the number of items of the curve reaches bufferSize, the oldes item
      * is removed.
      *
      * @see setBufferSize
      */
    int bufferSize() const;

    /** \brief the curve name
      *
      * Each curve has a name by means of which it is identified by the owning plot (PlotSceneWidget)
      */
    QString name() const;

    virtual void addPoint(double x, double y);

    virtual void addPoints(const QVector<double>& xData, const QVector<double> &yData);

    Data *data() const;

    void setData(const QVector<double>& xData, const QVector<double> &yData);

    void setData(const QVector<double> &yData);

    void installCurveChangeListener(CurveChangeListener *listener);

    QList<CurveChangeListener *>curveChangeListeners() const;

    void removeCurveChangeListener(CurveChangeListener *listener);

    void removeCurveItem();

    void setCurveItem(CurveItem *curveItem);

    CurveItem* curveItem() const;

    ScaleItem::Id associatedXAxisId() const;

    ScaleItem::Id associatedYAxisId() const;

    virtual double getMinX() const;

    virtual double getMinY() const;

    virtual double getMaxX() const;

    virtual double getMaxY() const;

    ScaleItem* getXAxis() const;

    ScaleItem* getYAxis() const;

    void invalidateCache();

    void invalidateXCache();

    void invalidateYCache();

    void xAxisBoundsChanged(double xl, double xu);

    void yAxisBoundsChanged(double yl, double yu);

    /** \brief when a scale item changes its autoscale state, if autoscale is enabled
      *        it is necessary to recalculate all max and min values of the curves.
      *
      *        Actually, if autoscale was previously disabled, at each addPoint call
      *        or addPoints call, the maximum and minimum values of the curves were not
      *        calculated.
      */
    void axisAutoscaleChanged(ScaleItem::Orientation o, bool autoscale);

    /** not needed. Empty bodies.
     */
    virtual void tickStepLenChanged(double  ) {}

    /** not needed. Empty bodies.
     */
    virtual void labelsFormatChanged(const QString& ) {}

    bool xDataIsOrdered() const;

    bool yDataIsOrdered() const;

    /** \brief returns a list of points in scene coordinates. Each point is the
      *        transformation in scene coordinates of the x and y corresponding
      *        value.
      *
      * @return a vector of QPointF representing the points of the curve in scene
      *         coordinates
      *
      * @param curve the SceneCurve which owns the data.
      *
      * <h3>Note</h3><p>Marks x and y pos cache as valid, because this method calculates
      * the position of each point in scene coordinates.
      * </p>
      */
    const QPointF* points();

    virtual void canvasRectChanged(const QRectF& newRect);

signals:
    
public slots:

    void setBufferSize(int bufSiz);

    void setXDataIsOrdered(bool ordered);

    void setYDataIsOrdered(bool ordered);

protected:

private:


    int mCheckBufferSize();

    bool mRemovedItemAffectsBounds(const Point &toRemovePt);

    SceneCurvePrivate *d_ptr;

    Q_DECLARE_PRIVATE(SceneCurve)
    
};

#endif // SCENECURVE_H
