#ifndef PLOTGEOMETRYEVENTLISTENER_H
#define PLOTGEOMETRYEVENTLISTENER_H

#include <QRectF>

/** \brief This is an interface that you can subclass to be notified when the plot rect
  *        changes.
  *
  * Subclass PlotEventListener and implement the plotRectChanged method if you want to be
  * notified when the plot rect changes.
  *
  * This usually happens when the scene rect changes or the plot area changes.
  *
  * @see PlotSceneWidget::installPlotChangeListener
  * @see PlotSceneWidget::setPlotAreaPercentage
  * @see PlotSceneWidget::setPlotAreaTopLeftPercentage
  * @see PlotSceneWidget
  */
class PlotGeometryEventListener
{
public:
    PlotGeometryEventListener() {}

    /** \brief pure virtual method that needs to be reimplemented in order to be
      *        notified when the plot rect changes
      *
      * @param newRect the new plot rectangle.
      *
      * @see PlotSceneWidget::installPlotChangeListener
      * @see PlotSceneWidget::setPlotAreaPercentage
      * @see PlotSceneWidget::setPlotAreaTopLeftPercentage
      * @see PlotSceneWidget
    */
    virtual void plotRectChanged(const QRectF &newRect) = 0;

    /** \brief pure virtual method that needs to be reimplemented in order to be
      *        notified when the plot area changes.
      *
      * \note The area takes into account the scaling factors currently applied to
      *       the view, i.e. m11() and m22() of the current transformation obtained
      *       by transform().
      *
      */
    virtual void plotAreaChanged(const QSizeF &area) = 0;

    /** \brief pure virtual method that needs to be reimplemented in order to be
      *        notified when the value of one of the two scrollbars changes.
      *
      * @param orientation one of the two possible Qt::Orientation values: Qt::Horizontal
      *        or Qt::Vertical
      *
      * @param value the value of the scrollbar
      */
    virtual void scrollBarChanged(Qt::Orientation orientation, int value) = 0;

    virtual void plotZoomLevelChanged(int level ) = 0;
};

#endif // PLOTCHANGELISTENER_H
