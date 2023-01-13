#ifndef MOUSEEVENTLISTENER_H
#define MOUSEEVENTLISTENER_H

class QGraphicsSceneMouseEvent;
class QPointF;
class QGraphicsPlotItem;

/** \brief an abstract class that can be used to be notified when a mouse event takes place
  *
  * Mouse press, move, release and click event can be intercepted.
  * In each method, a reference to the PlotSceneWidget that received the event is passed.
  */
class MouseEventListener
{
public:
    MouseEventListener() {}

    virtual void mousePressEvent(QGraphicsPlotItem *, QGraphicsSceneMouseEvent *) {}

    virtual void mouseReleaseEvent(QGraphicsPlotItem *, QGraphicsSceneMouseEvent *) {}

    virtual void mouseMoveEvent(QGraphicsPlotItem *, QGraphicsSceneMouseEvent *) {}

    virtual void mouseClickEvent(QGraphicsPlotItem *, const QPointF& ) {}

    virtual void mouseDoubleClickEvent(QGraphicsPlotItem *, QGraphicsSceneMouseEvent *) {}
};

#endif // MOUSEEVENTLISTENER_H
