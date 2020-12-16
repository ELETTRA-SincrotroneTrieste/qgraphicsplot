#ifndef TARGETITEM_H
#define TARGETITEM_H

#include <QGraphicsObject>
#include <axischangelistener.h>

class ItemMoveListener;
class ScaleItem;
class PlotSceneWidget;
class TargetItemPrivate;

/** \brief A QGraphicsObject that can be moved and can be attached to a writer
  *        in order to write a value according to the position of the item.
  *
  * A TargetItem must be attached to a couple of ScaleItem axes, and must be
  * constructed passing a reference to a PlotSceneWidget, as you can see in
  * the TargetItem constructor.
  *
  * @see TargetItem
  *
  * In order to receive move events, you have to implement the ItemMoveListener
  * interface and install it by calling installItemMoveListener.
  * Move events are sent only if the mouse button is pressed. setPos does not per se
  * produce a move event.
  *
  * @see installItemMoveListener
  * @see ItemMoveListener
  *
  * \par Automatic repositioning of the TargetItem when the plot geometry changes.
  * TargetItem implements AxisChangeListener interface.
  * When the PlotSceneWidget plot canvas changes its geometry, the canvasRectChanged method
  * repositions the target item according to the new geometry and the values of x and y
  * axes coordinates of the target items, which are set with setX and setY.
  *
  * \par Note
  * The TargetItem constructor <strong>installs the TargetItem on each ScaleItem
  * in order to receive the geometry change events</strong> of the PlotSceneWidget
  * and to be able to reposition itself when the canvas rect changes.
  *
  */
class TargetItem : public QGraphicsObject, public AxisChangeListener
{
    Q_OBJECT
public:

    enum { Type = UserType + 101 };

    /* Enable the use of qgraphicsitem_cast with this item. */
    virtual int type() const
    {
        return Type;
    }

    TargetItem(ScaleItem* xAxis, ScaleItem* yAxis, PlotSceneWidget* plot);

    QColor circleColor() const;

    bool pointLabelEnabled() const;

    double innerRadius() const;

    double outerRadius() const;

    double radiusScaleDivider() const;

    double x() const;

    double y() const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    QRectF boundingRect () const;

    void installItemMoveListener(ItemMoveListener *l);

    ItemMoveListener *itemMoveListener() const;

    void canvasRectChanged(const QRectF &newRect);

    /** \brief This must be implemented from AxisChangeListener interface, but they do nothing.
     *
     * Calls updatePosition();
     */
    virtual void xAxisBoundsChanged(double, double);

    /** \brief This is aimed at repositioning the target item.
     *
     * Calls updatePosition();
     */
    virtual void yAxisBoundsChanged(double, double);

    /** \brief This is aimed at repositioning the target item.
      */
    virtual void axisAutoscaleChanged(ScaleItem::Orientation, bool ) {}

    virtual void tickStepLenChanged(double) {}

    virtual void  labelsFormatChanged(const QString &) {}

public slots:

    void setCircleColor(const QColor &c);

    void setPointLabelEnabled(bool en);

    void setInnerRadius(double r);

    void setOuterRadius(double r);

    void setRadiusScaleDivider(double divider);

    void setX(double x);

    void setY(double y);

    void updatePosition();

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    TargetItemPrivate *d_ptr;

    Q_DECLARE_PRIVATE(TargetItem)
};

#endif // TARGETITEM_H
