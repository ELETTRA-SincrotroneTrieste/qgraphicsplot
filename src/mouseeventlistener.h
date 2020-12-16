#ifndef MOUSEEVENTLISTENER_H
#define MOUSEEVENTLISTENER_H

class QMouseEvent;
class QPointF;
class PlotSceneWidget;

/** \brief an abstract class that can be used to be notified when a mouse event takes place
  *
  * Mouse press, move, release and click event can be intercepted.
  * In each method, a reference to the PlotSceneWidget that received the event is passed.
  */
class MouseEventListener
{
public:
    MouseEventListener() {}

    virtual void mousePressEvent(PlotSceneWidget *, QMouseEvent *) {}

    virtual void mouseReleaseEvent(PlotSceneWidget *, QMouseEvent *) {}

    virtual void mouseMoveEvent(PlotSceneWidget *, QMouseEvent *) {}

    virtual void mouseClickEvent(PlotSceneWidget *, const QPointF& ) {}

    virtual void mouseDoubleClickEvent(PlotSceneWidget *, QMouseEvent *) {}
};

#endif // MOUSEEVENTLISTENER_H
