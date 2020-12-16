#ifndef CURVEITEM_H
#define CURVEITEM_H

#include <QGraphicsObject>
#include <QAbstractGraphicsShapeItem>
#include "axischangelistener.h"
#include "curvechangelistener.h"
#include <QPen>

class SceneCurve;
class CurveItemPrivate;
class ItemPainterInterface;

/** \brief A QGraphicsObject that allows an implementation of the ItemPainterInterface
 *         to draw on a PlotSceneWidget.
           If an ItemPainterInterface (or more) is installed, then the draw method of the
           ItemPainterInterface is invoked instead of the default paint method.

  *
  * @see ItemPainterInterface
  * @see SceneCurve
  * @see PlotSceneWidget
  *
  * The CurveItem class calculates the necessary transformations that allow to correctly
  * map the real data values to the plot canvas coordinates.
  * It makes use of classes implementing the ItemPainterInterface interface in order to
  * draw the data on the plot canvas.
  *
  * \par Example
  * Read the SceneCurve documentation for an example.
  *
  * \par Item painters ownership
  * The CurveItem takes the ownership of every installed ItemPainterInterface implementation
  * instance. This means that all the installed ItemPainterInterface objects are deleted on
  * CurveItem deletion.
  *
  * \par CurveItem ownership
  * If you call PlotSceneWidget::removeCurve, then the associated CurveItem is deleted.
  * If you delete a SceneCurve without invoking PlotSceneWidget::removeCurve (bad), then
  * the associated CurveItem is deleted again for you.
  * This means that once installed on a SceneCurve, the ownership of the CurveItem is
  * taken by the SceneCurve. With the term <em>install</em> we mean the
  *
  * \code
  * CurveItem *curveItem = new CurveItem(curve);
  * \endcode
  * instruction.
  *
  */
class CurveItem : public QGraphicsObject, public CurveChangeListener
{

    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)
    Q_PROPERTY(qreal zValue READ zValue WRITE setZValue)

    Q_OBJECT
public:

    CurveItem(SceneCurve *curve);

    virtual ~CurveItem();

    SceneCurve *sceneCurve() const;

    virtual QRectF boundingRect() const;

    virtual void itemAboutToBeAdded(const Point &) {}

    virtual void itemAdded(const Point &pt);

    virtual void itemAboutToBeRemoved(const Point &pt);

    virtual void itemRemoved(const Point &) {}

    virtual void bufferSizeChanged(int ) {}

    virtual void minXChanged(double ) {}

    virtual void maxXChanged(double ) {}

    virtual void minYChanged(double  )  {}

    virtual void maxYChanged(double ) {}

    virtual void fullVectorUpdate();

    virtual void affectingBoundsPointsRemoved() {}

    void installItemPainterInterface(ItemPainterInterface *itemPainterInterface);

    void removeItemPainterInterface(ItemPainterInterface *itemPainterInterface);

    QList<ItemPainterInterface *>itemPainters() const;

    ItemPainterInterface *itemPainter() const;

protected:
    /** \brief draws the lines connecting the XYItems in the associated curve
      *
      */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);


private:

    Q_DECLARE_PRIVATE(CurveItem)

    CurveItemPrivate *d_ptr;

};

#endif // CURVEITEM_H
