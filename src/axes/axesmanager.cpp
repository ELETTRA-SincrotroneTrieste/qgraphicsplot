#include "axesmanager.h"
#include "scaleitem.h"
#include "qgraphicsplotmacros.h"
#include <QSet>
#include <QtDebug>

/*!
 * \brief The constructor. It takes no parameters.
 *
 * Constructed and used by PlotSceneWidget.
 * You can retrieve a reference by calling PlotSceneWidget::axesManager.
 *
 * @see PlotSceneWidget
 */
AxesManager::AxesManager()
{
}

void AxesManager::removeDuplicateCouples()
{
    QList<AxisCouple> coupleSet;
    foreach(AxisCouple ac, axes)
    {
        if(!coupleSet.contains((ac)))
            coupleSet.append(ac);
    }
    axes = coupleSet;
}

/** \brief returns the ScaleItem with the axis id passed as parameter
  *
  * @param axisId the id of the axis you want to retrieve
  *
  * @return the ScaleItem with the specified axisId.
  * @return NULL if no ScaleItem was found with the specified axisId
  */
ScaleItem *AxesManager::getAxis(ScaleItem::Id axisId) const
{
    foreach(AxisCouple ac, axes)
    {
        if(ac.xAxis->axisId() == axisId)
            return ac.xAxis;
        else if(ac.yAxis->axisId() == axisId)
            return ac.yAxis;
    }
    return NULL;
}

/** \brief returns a list of all the axes in the PlotSceneWidget
  *
  * For each AxisCouple in the PlotSceneWidget, it returns its x and y
  * ScaleItem references.
  *
  */
QList<ScaleItem *> AxesManager::getAllAxes() const
{
    QList<ScaleItem *> list;
    foreach(AxisCouple ac, axes)
    {
        if(ac.xAxis != NULL)
            list << ac.xAxis;
        if(ac.yAxis != NULL)
            list << ac.yAxis;
    }
    return list;
}

/** \brief Returns a list of axes with the specified orientation, or all axes is all is true
 *
 */
QList<ScaleItem *> AxesManager::getAxes(bool all, ScaleItem::Orientation o) const
{
    QList<ScaleItem *> items;
    foreach(ScaleItem* s, getAllAxes())
    {
        if(all || o == s->orientation())
            items << s;
    }
    return items;
}

void AxesManager::clearAxes(bool all, ScaleItem::Orientation o)
{
    QSet<ScaleItem *> deletedAxes;
    ScaleItem* s;
    for(int i = 0; i < axes.size(); i++)
    {
        AxisCouple &ac = axes[i];
        s = ac.xAxis;
        if(s != NULL && (all || o == ScaleItem::Horizontal) )
        {
            if(!deletedAxes.contains(s))
            {
                deletedAxes.insert(s);
                delete s;
            }
            ac.xAxis = NULL;
        }
        s = ac.yAxis;
        if(s != NULL && (all || o == ScaleItem::Vertical) )
        {
            if(!deletedAxes.contains(s))
            {
                deletedAxes.insert(s);
                delete s;
            }
            ac.yAxis = NULL;
        }
    }
    if(all)
        axes.clear();
    else
        removeDuplicateCouples();
}

/** \brief given an axis with otherAxisId as id, this method returns the associated axis
  *
  * @return the axis associated to the axis identified by otherAxisId.
  *
  * Axes are coupled in two. This method returns the one coupled with otherAxisId
  */
ScaleItem *AxesManager::getAssociatedAxis(ScaleItem::Id otherAxisId)
{
    for(int i = 0; i < axes.size(); i++)
    {
        AxisCouple ac = axes.at(i);
        if(ac.xAxis != NULL && ac.xAxis->axisId() == otherAxisId)
            return ac.yAxis;
        else if(ac.yAxis != NULL && ac.yAxis->axisId() == otherAxisId)
            return ac.xAxis;
    }
   // perr("AxesManager::getAssociatedAxis: no axis is associated to axis id %d", otherAxisId);
    return NULL;
}

/** \brief Given the axis id of two axes, it returns a pair of double representing
  *        the position, given in terms of percentage, of the x and y axis with respect
  *        to the scene.
  *
  */
QPair<double, double> AxesManager::getOriginPosPercentage(ScaleItem::Id oneAxisId, ScaleItem::Id otherAxisId, bool *ok)
{
    QPair<double, double> oPosPercent;
    *ok = true;
    for(int i = 0; i < axes.size(); i++)
    {
        AxisCouple ac = axes.at(i);
  //      printf("axes %i : x: %d y %d one %d other %d [%f,%f]\e[0m\n", i, ac.xAxis->axisId(),
  //             ac.yAxis->axisId() , oneAxisId, otherAxisId,
  //             ac.xOriginPosPercentage, ac.yOriginPosPercentage);
        /* no importance is given to the order of the parameters */
        if((ac.xAxis->axisId() == oneAxisId && ac.yAxis->axisId() == otherAxisId) ||
                (ac.xAxis->axisId() == otherAxisId && ac.yAxis->axisId() == oneAxisId))
        {
            oPosPercent.first = ac.xOriginPosPercentage;
            oPosPercent.second = ac.yOriginPosPercentage;
            return oPosPercent;
        }

    }
    *ok = false;
  //  perr("AxesManager::getOrigin: the two axis id do not represent coupled axes");
    return oPosPercent;
}

/** \brief Returns true if an incomplete couple has been found, false otherwise.
 *
 * An incomplete couple is an AxisCouple that has just one of the two axis not NULL.
 */
AxisCouple * AxesManager::findIncompleteCouple(ScaleItem *item)
{
    AxisCouple *ret = NULL;
    for(int i = 0; i < axes.size(); i++)
    {
        AxisCouple &ac = axes[i];
        if(ac.xAxis == item && ac.yAxis == NULL)
            ret = &ac;
        else if(ac.yAxis == item && ac.xAxis == NULL)
            ret = &ac;
    }
    return ret;
}

int AxesManager::count() const
{
    return axes.size();
}
