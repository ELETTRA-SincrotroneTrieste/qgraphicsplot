#ifndef AXESMANAGER_H
#define AXESMANAGER_H

#include <QList>
#include <QPair>
#include <axiscouple.h>
#include <scaleitem.h>

/**
 * \brief Manages the axes of a PlotSceneWidget
 *
 */
class AxesManager
{
public:

    AxesManager();

    void addCouple(const AxisCouple &c) { axes.append(c); }

    void removeCouple(const AxisCouple &c) { axes.removeAll(c); }

    void removeDuplicateCouples();

    ScaleItem *getAxis(ScaleItem::Id axisId) const;

    /** \brief returns the list of axis couples in the plot.
      *
      * @return the list of axis couples in the plot.
      *
      */
    QList<AxisCouple>& getAxisCouples()  { return axes; }

    QList<ScaleItem *> getAllAxes() const;

    QList<ScaleItem *> getAxes(bool all, ScaleItem::Orientation o) const;

    void clearAxes(bool all, ScaleItem::Orientation o);

    /** \brief Given an axis, identified by its Id, this method finds and returns
      *        the <strong>first</strong> axis associated to the otherAxis axis.
      *
      * <h3>Note</h3><p>If one axis is shared through different couples, (for instance
      * an x axis common between two distinct y axes), then the associated axis in the
      * first couple in the list is returned.
      * </p>
      */
    ScaleItem *getAssociatedAxis(ScaleItem::Id otherAxis);

    /** \brief given two axes, this method looks for the couple inside the AxisCouple
      *        list and returns the associated origin.
      *
      * @param oneAxisId one of the axis id to look for
      * @param otherAxisId the other axis id to look for (the order is irrelevant)
      * @param a compulsory parameter used to test whether the two axes are associated
      *        and the origin is defined.
      *
      * @return the point representing the origin between the axes xA and yA, if they
      *         belong to the same couple. Returns NULL if the two axes do not belong
      *         to the same couple.
      *
      * @see getAssociatedAxis
      */

    QPair<double, double> getOriginPosPercentage(ScaleItem::Id oneAxisId, ScaleItem::Id otherAxisId, bool *ok);

    AxisCouple *findIncompleteCouple(ScaleItem *item);

    int count() const;

    QList<AxisCouple> axes;
};

#endif // AXESMANAGER_H
