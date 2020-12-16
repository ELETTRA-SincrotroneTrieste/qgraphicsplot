#ifndef SCALELABELINTERFACE_H
#define SCALELABELINTERFACE_H

#include <QString>

/** \brief This interface defines methods to provide custom labels in a ScaleItem
  *
  * To customize axis labels, just provide an implementation of this ScaleLabelInterface
  * and install it on a ScaleItem.
  * You must implement the label method, that returns a QString depending on the value of
  * the point in the axis.
  *
  * \par Example
  * A custom implementation may return a string displaying a date/time value
  * represented by a timestamp on the x axis.
  *
  */
class ScaleLabelInterface
{
public:
    ScaleLabelInterface();

    enum Type { Undefined = -1, TimeScale, Position = 1, ZPosition, XPosition, YPosition, LabelMap,
                User = 100, MaxUser = 1000 };

    /** \brief This method must be reimplemented in a subclass and must return a QString
      *        to be displayed in the ScaleItem in correspondance to the value on the axis.
      *
      * @param value the value on the ScaleItem, in the ScaleItem coordinates.
      * @return a QString that represents the value passed as parameter.
      *
      * @see ScaleItem::installScaleLabelInterface
      * @see ScaleItem
      */
    virtual QString label(double value) const = 0;

    virtual Type type() const = 0;

    /** \brief This method returns the longest possible string that may be returned by
     *         the implementing scale label interface.
     *
     * \par Note: this method must return the longest possible string independently of the
     *      labels currently displayed as returned by the label method. For instance, if the
     *      x axis maps the months of the year, then "September" must be returned.
     *
     * \par Note: if the longestLabel changes over time (for instance, the format of the months
     *      switches to the month number instead of the name), then a call to updateGeometry may be required
     *      on the scales that use the ScaleLabelInterface, in order to recalculate the size
     *      occupied by the labels.
     */
    virtual QString longestLabel() const = 0;

};

#endif // SCALELABELINTERFACE_H
