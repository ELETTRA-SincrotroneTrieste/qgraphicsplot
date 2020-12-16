#include "timescalelabel.h"

#include <QDateTime>
#include <QtDebug>

TimeScaleLabel::~TimeScaleLabel()
{
    delete d_ptr;
}

TimeScaleLabel::TimeScaleLabel(bool showDate)
{
    d_ptr = new TimeScaleLabelPrivate();
    d_ptr->showDate = showDate;
}

QString TimeScaleLabel::label(double value) const
{
    QString ret;
    if(!d_ptr->showDate)
        ret = QDateTime::fromTime_t(value).toString("hh:mm:ss");
    else
        ret = QDateTime::fromTime_t(value).toString("dd/MM hh:mm:ss");
    return ret;
}

/** \brief show the date together with the time
 *
 * @param show true show date and time
 * @param show false show only the time
 *
 * \par Note
 * updateGeometry may be required on the scale that uses this interface
 */
void TimeScaleLabel::setShowDate(bool show)
{
    d_ptr->showDate = show;
}

/** \brief Returns true if the label contains both time and date, false
 *         if it shows only the time
 *
 */
bool TimeScaleLabel::showDate() const
{
    return d_ptr->showDate;
}

ScaleLabelInterface::Type TimeScaleLabel::type() const
{
    return ScaleLabelInterface::TimeScale;
}

/** \brief returns the longest possible string returned by the label method
 *
 */
QString TimeScaleLabel::longestLabel() const
{
    if(!d_ptr->showDate)
        return "XX:XX:XX";
    else
        return "XX/MMHH:MM:SS";
}

