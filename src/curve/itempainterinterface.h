#ifndef ITEMPAINTERINTERFACE_H
#define ITEMPAINTERINTERFACE_H

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class Point;
class SceneCurve;
class QGraphicsPlotItem;
class QGraphicsObject;
class CurveItem;
class QPen;

#include <QRectF>

/** \brief A class that can be installed in a CurveItem which is delegated to
  *        drawing.
  *
  */
class ItemPainterInterface
{
public:

    enum Type { Line = 0, Dot, Cross, Histogram, Step, Pie, CircleItemSet, User = 100 };

    ItemPainterInterface()
    {
    }

    virtual ~ItemPainterInterface()
    {

    }

    /** \brief the drawing method.
      *
      * This method is invoked by CurveItem inside paint method.
      * You can obtain the points of the curve in scene coordinates by calling
      * curve->points and the size of the points by means of curve->dataSize.
      * It may be necessary to call painter->save() and painter->restore() at the beginning
      * and end of the drawing. Each draw method of each installed ItemPainterInterface is
      * actually invoked in sequence without saving and restoring the painter state for you.
      *
      * @see CurveItem
      * @see LinePainter
      * @see DotsPainter
      */
    virtual void draw(SceneCurve *curve,
                      QPainter *painter,
                      const QStyleOptionGraphicsItem * option,
                      QWidget * widget = 0) = 0;

    virtual CurveItem *curveItem() const = 0;

    /** \brief the size of each drawn element
      *
      * This size is used by the classes that use ItemPainterInterface to know how much
      * extra space is needed in order to calculate the bounding rect of the CurveItem
      * or other geometrical properties.
      *
      * For instance, if you are drawing circles, then this method should return the radius
      * of the circle, so that CurveItem knows how much space has to be added to the curve
      * bounds in order to correcly draw the items.
      *
      * If you are drawing lines, then this size can be 0x0 since the connecting lines do not
      * require extra space to be calculated. If you use a particular pen width, then you may
      * want to return its width in order to be sure that CurveItem takes care of this space.
      *
      * @see CurveItem
      */
    virtual QSizeF elementSize() const = 0;

    virtual int type() const = 0;

    virtual QPen pen() const = 0;

};

#endif // ITEMPAINTERINTERFACE_H
