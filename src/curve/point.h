#ifndef POINT_H
#define POINT_H

class PointPrivate;
class PlotSceneWidget;
class PointData;
class ScaleItem;
class ItemPositionChangeListener;

#include <QPointF>
#include <QList>

/** \brief a point item that calculates its position in the view
  *        and simply draws a cross centered at x,y and the represented
  *        x and y values.
  *
  * You must inherit from this class or install a custom ItemPainterInterface to
  * change its drawing behaviour.
  */
class Point
{
    friend class SceneCurve;
public:

    explicit Point(const PointData& data, ScaleItem* xAxis, ScaleItem *yAxis);

    Point(double x, double y);

    virtual ~Point();

    double updateXPos(PlotSceneWidget *view);

    double updateYPos(PlotSceneWidget *view);

    /** \brief calculates the position in scene coordinates according to the view
      *        geometry.
      *
      * Updates the internal position available through the pos call.
      */
    QPointF updatePosition(PlotSceneWidget *view);

    /** \brief returns the position of the point in scene coordinates
      *
      * @return the QPointF representing the item position inside the scene,
      *         in scene coordinates.
      */
    QPointF pos() const;

    const PointData &pointDataR() const;

    PointData pointData() const;

    /** \brief sets the PointData associated to the XYItem.
      *
      * Sets or updates the PointData associated to the item.
      * Remember to call calculatePosition after this call if you need to
      * reposition the item after its data has changed.
      */
    void setPointData(const PointData& pd);

    void setPos(const QPointF &pos);

    void setPos(qreal x, qreal y);

    double x, y;

    double xpos, ypos;

protected:



private:
    PointPrivate *d_ptr;

    Q_DECLARE_PRIVATE(Point)
};



#endif // XYITEM_H
