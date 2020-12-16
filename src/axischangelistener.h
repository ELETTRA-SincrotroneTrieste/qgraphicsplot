#ifndef AXISCHANGELISTENER_H
#define AXISCHANGELISTENER_H

#include <scaleitem.h>

/** \brief A listener for axis change events
  *
  * Implement this interface in a subclass in order to receive ScaleItem events,
  * such as bound changes and auto scale property changes.
  *
  * Install your object by calling ScaleItem::installAxisChangeListener
  *
  */
class AxisChangeListener
{
public:
    AxisChangeListener() {}

    /** \brief x axis bounds notification
      *
      * @param lower the new lower bound
      * @param upper the new upper bound
      */
    virtual void xAxisBoundsChanged(double lower, double upper) = 0;

    /** \brief y axis bounds notification
      *
      * @param lower the new lower bound
      * @param upper the new upper bound
      */
    virtual void yAxisBoundsChanged(double lower, double upper) = 0;

    /** \brief x axis auto scale property change notification
      *
      * @param o the ScaleItem::Orientation property that helps you find out which axis
      *        (x or y) has changed its property.
      *
      * @param autoscale true autoscale is enabled.
      * @param autoscale false autoscale is disabled.
      */
    virtual void axisAutoscaleChanged(ScaleItem::Orientation o, bool autoscale) = 0;

    /** \brief notifies when the canvas area changes.
      *
      * In some cases, when the axis is placed on the right or at the bottom
      * of the plot, the labels might require more space, and the plot area
      * must be reduced.
      *
      * @param newRect the new rectangle
      */
    virtual void canvasRectChanged(const QRectF& newRect) = 0;

    /** \brief notifies listeners about tick step len changes.
     *
     * The tick step len can be tuned by a property in a ScaleItem or let it
     * be automatically calculated by ScaleItem.
     * @see ScaleItem
     */
    virtual void tickStepLenChanged(double value ) = 0;

    /** \brief when the labels format changes, labelsFormatChanged is used
     *
     * @see ScaleItem
     */
    virtual void labelsFormatChanged(const QString& newFormat) = 0;
};

#endif // AXISCHANGELISTENER_H
