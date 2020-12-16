#include "scaleitem.h"
#include "scaleitemprivate.h"
#include "curve/point.h"
#include "curve/pointdata.h"
#include "curve/scenecurve.h"
#include "scalelabelinterface.h"
#include "curve/curveitem.h"
#include <stdio.h>
#include <QGraphicsScene>
#include <QDateTime>
#include <QPainter>
#include "plotscenewidget.h"

#include "qgraphicsplotmacros.h"
#include <math.h>
#include <QFont>
#include <QStyleOptionGraphicsItem>
#include <QtDebug>

ScaleItem::~ScaleItem()
{
    qDebug() << __FUNCTION__ << "deleting ScaleItem " << objectName();
}

/** \brief Constructs a scale item with an Orientation, a reference to the PlotSceneWidget
  *        and an id aimed at identifying the axis.
  *
  * The PlotSceneWidget creates two ScaleItem objects for you, an x item and an y item.
  * They are laid out so that they cross each other in the middle
  * (see PlotSceneWidget::setOriginPosPercentage, PlotSceneWidget::setDefaultXAxisOriginPosPercentage
  * and PlotSceneWidget::setDefaultYAxisOriginPosPercentage)
  *
  * If you want to customize the scale labels, then subclass ScaleLabelInterface and provide a
  * method named label in order to tell the ScaleItem what text to draw in correspondance to a
  * specific value on the axis.
  * @see ScaleLabelInterface
  *
  * The ScaleItem is a QGraphicsObject which is added to the scene of the PlotSceneWidget.
  *
  * The ScaleItem::Id is an enum with a bunch of default items. PlotSceneWidget manages the
  * two default axes for you. If you want to add more axes, please have a look to the
  * ScaleItem::Id enum and use one of the predefined values or add others by yourself.
  *
  */
ScaleItem::ScaleItem(Orientation o, PlotSceneWidget *view, ScaleItem::Id id)
    : QGraphicsObject(0),
      PlotGeometryEventListener()
{
    d_ptr = new ScaleItemPrivate();
    d_ptr->orientation = o;
    d_ptr->view = view;
    d_ptr->axisId = id;
    d_ptr->scaleLabelInterface = NULL;
    d_ptr->axisLabelsOutsideCanvas = false;
    d_ptr->tickWidth = 10;
    d_ptr->axisLabelDist = d_ptr->tickWidth;
    d_ptr->maxLabelWidth = -1.0;
    d_ptr->labelHeight = 10;
    d_ptr->labelMargin = 5.0;
    d_ptr->font = view->font();
    d_ptr->axisTitleFont = d_ptr->font;

    d_ptr->axisTitleFont.setItalic(true);
    d_ptr->font.setPointSizeF(d_ptr->fontSize);
    /* Free Sans scales well */
    d_ptr->font.setFamily("FreeSans");
    canvasWidth = view->plotRect().width();
    canvasHeight = view->plotRect().height();
    d_ptr->actualTickStepLen = updateStepLen();
    if(o == Vertical)
    {
        d_ptr->axisTitleFont.setBold(true);
        d_ptr->axisTitleFont.setPointSizeF(d_ptr->axisTitleFont.pointSizeF() + 1);
        d_ptr->axisLabelRotation = 0.0;
        /* first draw Y axis, so that X axis labels are not covered by the Y axis
         * grid. Actually, X axis labels may be time scales and so it is ugly to
         * have them cut by the Y grid.
         */
        setZValue(-1);
    }
    d_ptr->mAxisTitleHeight = 0.0;
    d_ptr->mAxisTitleWidth = 0.0;
    /* if this axis is created after plot has been setup, then let the plot rect be
     * initialized from the current plot rect
     */
    d_ptr->plotRect = view->plotRect();
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
}

/** \brief Install an AxisChangeListener in order to be notified when axis bounds change
  *        or the auto scale property changes.
  *
  * @param l an implementation of the AxisChangeListener interface
  * @see AxisChangeListener
  */
void ScaleItem::installAxisChangeListener(AxisChangeListener *l)
{
    qDebug() << __PRETTY_FUNCTION__ << l;
    /* add new listener */
    d_ptr->axisChangeListeners.append(l);

    /* initialize each listener with the currently initialized data.
     * Actually, not all curves may be added at once; instead, curves
     * can be added at any point in time, and so they need be initialized
     * with the current canvas rect, bounds and autoscale properties
     */
    l->canvasRectChanged(d_ptr->canvasRect);
    if(d_ptr->orientation == Horizontal)
        l->xAxisBoundsChanged(d_ptr->lowerBound, d_ptr->upperBound);
    else
        l->yAxisBoundsChanged(d_ptr->lowerBound, d_ptr->upperBound);
    /* listener (e.g. SceneCurve) will recalculate bounds if autoscale is enabled */
    l->axisAutoscaleChanged(d_ptr->orientation, d_ptr->autoScale);
    if(d_ptr->actualTickStepLen > -1)
        l->tickStepLenChanged(d_ptr->actualTickStepLen);
    if(!d_ptr->actualLabelsFormat.isEmpty())
        l->labelsFormatChanged(d_ptr->actualLabelsFormat);
}

/** \brief returns the label associated to the value passed as parameter, if a ScaleLabelInterface
  *        has been installed on the ScaleItem, the value itself otherwise.
  *
  * @param value the value on the ScaleItem whose label you want to retrieve
  * @return the label associated to the value value.
  *
  * @see installScaleLabelInterface
  * @see ScaleLabelInterface
  */
QString ScaleItem::label(double value) const
{
    if(d_ptr->scaleLabelInterface)
        return d_ptr->scaleLabelInterface->label(value);

    QString l;
    l.sprintf(qstoc(d_ptr->actualLabelsFormat), value);
    return l;
}

/** \brief Provide a custom label manager for the ScaleItem.
  *
  * If a ScaleLabelInterface implementation is provided, then the labels text
  * will be obtained by ScaleLabelInterface::label method.
  *
  * @see ScaleLabelInterface
  * @param iface an object implementing the ScaleLabelInterface interface
  *
  * The ScaleItem class does not take ownership of the scale label interface.
  * This means that removeScaleLabelInterface will not delete the installed interface
  * and that subsequent calls to installScaleLabelInterface will not delete previously
  * installed scale label interfaces.
  */
void ScaleItem::installScaleLabelInterface(ScaleLabelInterface *iface)
{
    d_ptr->scaleLabelInterface = iface;
    this->updateLabelsCache();
    update();
}

/** \brief removes the custom scale label interface implementation.
  *
  * If this method is called, then the ScaleItem labels text will simply display
  * the values of the x data.
  */
void ScaleItem::removeScaleLabelInterface()
{
    d_ptr->scaleLabelInterface = NULL;
    update();
}

/** \brief returns the ScaleLabelInterface that was installed with installScaleLabelInterface,
  *         null otherwise.
  *
  * @return the installed ScaleLabelInterface or NULL if it was not installed.
  * @see installScaleLabelInterface
  *
  */
ScaleLabelInterface *ScaleItem::scaleLabelInterface() const
{
    return d_ptr->scaleLabelInterface;
}

void ScaleItem::removeAxisChangeListener(AxisChangeListener *l)
{
    d_ptr->axisChangeListeners.removeAll(l);
}

void ScaleItem::setOrientation(Orientation o)
{
    d_ptr->orientation = o;
}

ScaleItem::Orientation ScaleItem::orientation() const
{
    return d_ptr->orientation;
}

ScaleItem::Id ScaleItem::axisId() const
{
    return d_ptr->axisId;
}

/** \brief returns the upper bound (maximum value) of the scale
  *
  *
  * Axis auto scale must be disabled otherwise this call will have no effect.
  *
  * @return the upper bound of the ScaleItem
  *
  * @see setUpperBound
  * @see setLowerBound
  * @see lowerBound
  * @see setAxisAutoscaleEnabled
  */
double ScaleItem::upperBound() const
{
    return d_ptr->upperBound;
}

/** \brief returns the lower bound (minimum value) of the scale
  *
  *
  * Axis auto scale must be disabled otherwise this call will have no effect.
  *
  * @return the lower bound of the ScaleItem
  *
  * @see setUpperBound
  * @see setLowerBound
  * @see upperBound
  * @see setAxisAutoscaleEnabled
  */
double ScaleItem::lowerBound() const
{
    return d_ptr->lowerBound;
}


/** \brief sets upper and lower bounds
  *
  * Axis auto scale must be disabled otherwise this call will have no effect.
  *
  * @see setUpperBound
  * @see setLowerBound
  *
  * \note recalculates the actual tick step len if not set by the user.
  */
void ScaleItem::setBounds(double lowerBound, double upperBound)
{
    if(lowerBound < upperBound)
    {
        if(d_ptr->lowerBound != lowerBound || d_ptr->upperBound != upperBound)
        {
            d_ptr->lowerBound = lowerBound;
            d_ptr->upperBound = upperBound;

            d_ptr->view->boundsChanged();

            /* needs to be called before updateLabelsFormat */
            updateStepLen();
            /* once the tick step len is up to date, update labels format */
            if(d_ptr->axisLabelsFormat.isEmpty()) /* avoid a function call if not needed */
                updateLabelsFormat(d_ptr->axisLabelsFormat);

            mNotifyBoundsChanged();

            /* labels width might have changed */
            updateLabelsCache();

            emit upperBoundChanged(upperBound);
            emit lowerBoundChanged(lowerBound);

            /* update */
            prepareGeometryChange();
        }
    }
    else
        perr("ScaleItem::setBounds: upper bound must be greater than lower bound!");
}

/** \brief sets the axis maximum value (upper bound)
  *
  * @param ub the value of the upper bound.
  *
  * axisAutoscaleEnabled property must be set to false.
  *
  * @see setLowerBound
  * @see upperBound
  * @see setAxisAutoscaleEnabled
  */
void ScaleItem::setUpperBound(double ub)
{
    if(ub > d_ptr->lowerBound)
    {
        d_ptr->upperBound = ub;
        d_ptr->view->boundsChanged();

        /* needs to be called before mUpdateLabelsFormat */
        updateStepLen();

        /* once the tick step len is up to date, update labels format */
        if(d_ptr->axisLabelsFormat.isEmpty()) /* avoid a function call if not needed */
            updateLabelsFormat(d_ptr->axisLabelsFormat);

        mNotifyBoundsChanged();

        /* labels width might have changed */
        updateLabelsCache();

        emit upperBoundChanged(ub);

        prepareGeometryChange();
    }
    else
        perr("ScaleItem::setUpperBound: upper bound must be greater than lower bound (which is %f)",
             d_ptr->lowerBound);
}

/** \brief sets the axis minimum value (lower bound)
  *
  * @param lb the value of the lower bound.
  *
  * axisAutoscaleEnabled property must be set to false.
  *
  * @see setUpperBound
  * @see upperBound
  * @see lowerBound
  * @see setAxisAutoscaleEnabled
  */
void ScaleItem::setLowerBound(double lb)
{
    if(lb < d_ptr->upperBound)
    {
        d_ptr->lowerBound = lb;
        d_ptr->view->boundsChanged();

        /* needs to be called before mUpdateLabelsFormat */
        updateStepLen();
        /* once the tick step len is up to date, update labels format */
        if(d_ptr->axisLabelsFormat.isEmpty()) /* avoid a function call if not needed */
            updateLabelsFormat(d_ptr->axisLabelsFormat);

        mNotifyBoundsChanged();

        /* labels width might have changed */
        updateLabelsCache();
        emit lowerBoundChanged(lb);
        prepareGeometryChange();
    }
    else
        perr("ScaleItem::setLowerBound: lower bound must be less than upper bound (which is %f)",
             d_ptr->upperBound);

}

/** \brief When in auto scale mode, the autoscaleMargin property defines a margin
 *  to apply to the span, expressed as percentage from 0 to 1 (0.1 is 10%).
 *
 * For example, if the scale range goes from 0 to 1000 and the autoscaleMargin is set to
 * 0.02 (2% is by the way the default setting), then the scale will start from -10 and end at 1010.
 * In this way, points that would be drawn in the very border of the plot will be more clearly
 * visible.
 */
void ScaleItem::setAutoscaleMargin(double spanPercent)
{
    d_ptr->autoscaleMargin = spanPercent;
}

double ScaleItem::autoscaleMargin() const
{
    return d_ptr->autoscaleMargin;
}

void ScaleItem::mNotifyBoundsChanged()
{
    foreach(AxisChangeListener* l, d_ptr->axisChangeListeners)
    {
        if(d_ptr->orientation == Horizontal)
            l->xAxisBoundsChanged(d_ptr->lowerBound, d_ptr->upperBound);
        else
            l->yAxisBoundsChanged(d_ptr->lowerBound, d_ptr->upperBound);
    }
}

/** \brief Sets the lower bound according to a given QDateTime timestamp.
 *
 * Sets the lower bound according to a given QDateTime timestamp.
 * @param t a QDateTime representing the lower bound of the scale.
 *
 * \note the QDateTime is converted into a double with the millisecond
 *       precision: t.toTime_t() + 0.001 * t.time().msec()
 *
 * \note This method is useful when your scale is a time scale.
 *
 * @see TimeScaleLabel
 *
 * @see setUpperBoundDateTime
 * @see lowerBoundDateTime
 *
 */
void ScaleItem::setLowerBoundDateTime(const QDateTime& t)
{
    setLowerBound(t.toTime_t() + 0.001 * t.time().msec());
}

/** \brief Sets the upper bound according to a given QDateTime timestamp.
 *
 * Sets the upper bound according to a given QDateTime timestamp.
 *
 * \note This method is useful when your scale is a time scale.
 *
 * @see setLowerBoundDateTime
 * @see lowerBoundDateTime
 *
 * @see TimeScaleLabel
 *
 */
void ScaleItem::setUpperBoundDateTime(const QDateTime& t)
{
    setUpperBound(t.toTime_t() + 0.001 * t.time().msec());
}

QDateTime ScaleItem::doubleToDateTime(double d) const
{
    double ms = d - floor(d);
    QDateTime t;
    t.setTime_t(floor(d));
    t = t.addMSecs(ms);
    return t;
}

/** \brief returns the lower bound as a QDateTime object
 *
 * @return the lower bound as a QDateTime object
 *
 * \par Conversion
 * The decimal part of the double value representing the axis bound
 * is used to set the milliseconds, the integer part is interpreted as
 * a unix timestamp, i.e. the number of seconds that have passed since
 * 1970-01-01T00:00:00, Coordinated Universal Time.
 *
 * @see setUpperBoundDateTime
 * @see setLowerBoundDateTime
 * @see upperBoundDateTime
 *
 * @see TimeScaleLabel
 */
QDateTime ScaleItem::lowerBoundDateTime() const
{
    return doubleToDateTime(d_ptr->lowerBound);
}

/** \brief returns the upper bound as a QDateTime object
 *
 * @return the upper bound as a QDateTime object
 * @see lowerBoundDateTime
 */
QDateTime ScaleItem::upperBoundDateTime() const
{
    return doubleToDateTime(d_ptr->upperBound);
}

/** \brief returns the value of the axisAutoscaleEnabled property
  *
  * @return true the axis scale is set according to the maximum value
  *         of the curves attached to the axis itself.
  * @return false the axis range is set according to upperBound and lowerBound
  *         properties.
  *
  * @see setUpperBound
  * @see setLowerBound
  * @see upperBound
  * @see lowerBound
  * @see setAxisAutoscaleEnabled
  *
  */
bool ScaleItem::axisAutoscaleEnabled() const
{
    return d_ptr->autoScale;
}

/** \brief enables or disables the axis auto scaling
  *
  *
  * @see setUpperBound
  * @see setLowerBound
  * @see upperBound
  * @see lowerBound
  * @see axisAutoscaleEnabled
  */
void ScaleItem::setAxisAutoscaleEnabled(bool en)
{
    d_ptr->autoScale = en;
    foreach(AxisChangeListener* l, d_ptr->axisChangeListeners)
        l->axisAutoscaleChanged(d_ptr->orientation, en);
    emit autoscaleEnabledChanged(en);
}

/** \brief defines the distance between the backbone and the start of the label's text
  *
  * @return the distance, in pixels, between the backbone of the axis and the start of text.
  *
  * The default value is 5
  *
  * @see setAxisLabelDist
  */
double ScaleItem::axisLabelDist() const
{
    return d_ptr->axisLabelDist;
}

/** \brief sets the distance between the backbone and the start of the label's text.
  *
  * @param d the distance, in scene coordinates, between the backbone and the start of text
  *        of each label.
  */
void ScaleItem::setAxisLabelDist(double d)
{
    d_ptr->axisLabelDist = d;
    scene()->update();
}

/** \brief Places the axis labels outside the plot canvas, i.e. on the left of the canvas.
  *
  * The axis labels are placed on the left of the plot rect.
  * The canvas rect left is moved accordingly, in order to define the effective area
  * where the curves can be drawn.
  *
  * @param outside true the axis is placed on the left of the plot rect. The plot canvas
  *        rect will be littler than the plot rect.
  *
  * @param outside false the axis is placed according to the
  *     PlotSceneWidget::originPosPercentage, PlotSceneWidget::defaultXAxisOriginPosPercentage
  *     and PlotSceneWidget::defaultYAxisOriginPosPercentage. In this case, the plot rect is
  *     equal to the plot canvas.
  *
  * @see axisLabelsOutsideCanvas
  * @see PlotSceneWidget::originPosPercentage
  * @see PlotSceneWidget::defaultXAxisOriginPosPercentage
  * @see PlotSceneWidget::defaultYAxisOriginPosPercentage
  */
void ScaleItem::setAxisLabelsOutsideCanvas(bool outside)
{
    d_ptr->axisLabelsOutsideCanvas = outside;
    scene()->update();
    /// updateLabelsCache(); /* updates full scene */
}

/** \brief returns the axisLabelsOutsideCanvas property.
  *
  * Read the setAxisLabelsOutsideCanvas documentation
  *
  * @see setAxisLabelsOutsideCanvas
  * @see PlotSceneWidget::originPosPercentage
  * @see PlotSceneWidget::defaultXAxisOriginPosPercentage
  * @see PlotSceneWidget::defaultYAxisOriginPosPercentage
  *
  */
bool ScaleItem::axisLabelsOutsideCanvas() const
{
    return d_ptr->axisLabelsOutsideCanvas;
}

/** \brief Returns the canvas rect, which is the rect where the curves
  *        can be drawn.
  *
  * This rect does not coincide with the plot rect if the axis labels
  * do not fit into the canvas.
  */
QRectF ScaleItem::canvasRect() const
{
    return d_ptr->canvasRect;
}

void ScaleItem::setTickStepLen(double sLen)
{
    d_ptr->tickStepLen = sLen;
    if(sLen != -1)
    {
        d_ptr->actualTickStepLen = sLen;
        foreach(AxisChangeListener* l, d_ptr->axisChangeListeners)
            l->tickStepLenChanged(sLen);
    }
    else
        d_ptr->actualTickStepLen = updateStepLen();

    /* ok, after d_ptr->actualTickStepLen is up to date */
    updateLabelsCache();

    prepareGeometryChange();
}

double ScaleItem::tickStepLen() const
{
    return d_ptr->tickStepLen;
}

void ScaleItem::setAxisLabelsEnabled(bool en)
{
    d_ptr->labelsEnabled = en;
    updateLabelsCache();
    prepareGeometryChange();
}

bool ScaleItem::axisLabelsEnabled() const
{
    return d_ptr->labelsEnabled;
}

void ScaleItem::setAxisLabelsRotation(double angle)
{
    d_ptr->axisLabelRotation = angle;
    redraw();
}

double ScaleItem::axisLabelsRotation() const
{
    return d_ptr->axisLabelRotation;
}

void ScaleItem::setAxisLabelsFormat(const QString& fmt)
{
    d_ptr->axisLabelsFormat = fmt;
    updateLabelsFormat(fmt);
    updateLabelsCache();
    prepareGeometryChange();
}

QString ScaleItem::axisLabelsFormat() const
{
    return d_ptr->axisLabelsFormat;
}

void ScaleItem::setGridEnabled(bool en)
{
    d_ptr->gridEnabled = en;
    redraw();
}

bool ScaleItem::gridEnabled() const
{
    return d_ptr->gridEnabled;
}

void ScaleItem::setGridColor(const QColor& c)
{
    d_ptr->gridColor = c;
    redraw();
}

void ScaleItem::setAxisColor(const QColor& c)
{
    d_ptr->axisColor = c;
    redraw();
}

QColor ScaleItem::axisColor() const
{
    return d_ptr->axisColor;
}

QColor ScaleItem::gridColor() const
{
    return d_ptr->gridColor;
}

void ScaleItem::setFont(const QFont& f)
{
    d_ptr->font = f;
    updateLabelsCache();
    scene()->update();
}

QFont ScaleItem::font() const
{
    return d_ptr->font;
}

QFont ScaleItem::axisTitleFont() const
{
    return d_ptr->axisTitleFont;
}

QColor ScaleItem::axisTitleColor() const
{
    return d_ptr->axisTitleColor;
}

QString ScaleItem::axisTitle() const
{
    return d_ptr->axisTitle;
}


void ScaleItem::setAxisTitleFont(const QFont &fo)
{
    d_ptr->axisTitleFont = fo;
    /* not to calculate font metrics at every refresh */
    mRecalculateAxisTitleSize();
}

void ScaleItem::setAxisTitleColor(const QColor & co)
{
    d_ptr->axisTitleColor = co;
}

void ScaleItem::setAxisTitle(const QString & ti)
{
    d_ptr->axisTitle = ti;
    mRecalculateAxisTitleSize();
}


double ScaleItem::maxLabelWidth() const
{
    return d_ptr->maxLabelWidth;
}

void ScaleItem::adjustScaleBounds(double newMin, double newMax)
{
    double min = d_ptr->lowerBound;
    double max = d_ptr->upperBound;

    if(newMin < min || d_ptr->minMaxUnset)
        min = newMin;
    if(newMax > max || d_ptr->minMaxUnset)
        max = newMax;
    d_ptr->minMaxUnset = false;
    if(d_ptr->lowerBound != min || d_ptr->upperBound != max)
        setBounds(min, max); /* updates tick step len and labels cache */
}

void ScaleItem::affectingBoundsPointsRemoved()
{
    if(d_ptr->autoScale && !d_ptr->view->inZoom())
    {
        printf("\e[0;32maffectingBoundsPointsRemoved().....\e[0m\n");
        /* must obtain again all maximum and minimum values from each curve */
        setBoundsFromCurves();
        /* correct scales if necessary */
        //    adjustScaleBounds();
    }
}

void ScaleItem::fullVectorUpdate()
{

}

void ScaleItem::plotRectChanged(const QRectF &newRect)
{
    QGraphicsView *view = NULL;
    if(scene()->views().size() > 0)
        view = scene()->views().first();
    //    QTransform tran = view->transform();
    /* initialize canvas rect, width and height to plot rect */
    //    qreal rectLeft, rectTop ,rectRight, rectBottom;

    //    rectLeft = tran.m11() * newRect.left() + tran.m21() * newRect.top() + tran.dx();
    //    rectTop = tran.m22() * newRect.top() + tran.m12() * newRect.left() + tran.dy();
    //    rectRight = tran.m11() * newRect.right() + tran.m21() * newRect.bottom() + tran.dx();
    //    rectBottom =  tran.m22() * newRect.bottom() + tran.m12() * newRect.right() + tran.dy();

    //   d_ptr->plotRect = QRectF:(QPointF(rectLeft, rectTop), QPointF(rectRight, rectBottom));

    //  qDebug() << this<<  "newRect " << newRect << " current rect " << d_ptr->plotRect;

    d_ptr->plotRect = newRect;

    redraw();
}

/** \brief Empty body. We are not interested in area changes.
 *
 * The area passed as parameter takes into account the scaling factor
 * of the view (not needed here).
 */
void ScaleItem::plotAreaChanged(const QSizeF &)
{
    updateStepLen();
    updateLabelsCache();
}

/** \brief is called when the plot zoom level changes.
 *
 * This method is called when the plot zoom level changes.
 *
 * @param level the level of the zoom
 *
 * \note Together with axes bound change, it is the third case in which
 * you need to calculate the tick step length again.
 *
 */
void ScaleItem::plotZoomLevelChanged(int level)
{
    d_ptr->plotZoomLevel = level;
    updateStepLen();
    updateLabelsCache();
    prepareGeometryChange();
}

/** \brief Empty body. We are not interested scroll bar changes.
 *
 */
void ScaleItem::scrollBarChanged(Qt::Orientation , int )
{

}

void ScaleItem::itemsAboutToBeDrawn()
{
    d_ptr->minMaxUnset = true;
}

int ScaleItem::mGetDecimals(double q)
{
    int decimals = 0;
    while(q < 1)
    {
        decimals++;
        q = q * 10;
    }
    return decimals;
}

void ScaleItem::mRecalculateAxisTitleSize()
{
    QFontMetrics fm(d_ptr->axisTitleFont);
    d_ptr->mAxisTitleHeight = fm.height();
    d_ptr->mAxisTitleWidth = fm.width(d_ptr->axisTitle);
}

/* updates the actualLabelsFormat private variable.
 *
 * If the desiredFormat hasn't been specified, then the method
 * needs the actualTickStepLen to be previously updated.
 *
 */
void ScaleItem::updateLabelsFormat(const QString& desiredFormat)
{
    QString previousFormat = d_ptr->actualLabelsFormat;
    if(!desiredFormat.isEmpty())
        d_ptr->actualLabelsFormat = desiredFormat;
    else
    {
        double x1, x2;
        x1 = d_ptr->lowerBound;
        x2 = d_ptr->upperBound;

        if(x2 >= x1)
        {
            qreal q = d_ptr->actualTickStepLen;
            if(q > 1 || q == 0)
                d_ptr->actualLabelsFormat = "%.0f";
            else
                d_ptr->actualLabelsFormat = QString("%.%1f").arg(mGetDecimals(q));
        }
    }
    /* notify axis label format has changed */
    if(previousFormat != d_ptr->actualLabelsFormat)
    {
        foreach(AxisChangeListener *l, d_ptr->axisChangeListeners)
            l->labelsFormatChanged(d_ptr->actualLabelsFormat);
    }
}

/* This method rebuilds the labels cache hash.
 * This method assumes that d)ptr->actualTickStepLen is up to date.
 * Call updateStepLen before if necessary
 *
 */
void ScaleItem::updateLabelsCache()
{
    double x, x0 = 0;
    double max = 0.0;
    double width;
    double x1 = d_ptr->lowerBound;
    double x2 = d_ptr->upperBound;
    /* assume that d_ptr->actualTickStepLen is up to date */
    double tickDist = d_ptr->actualTickStepLen;
    double originPercent;
    bool ok;
    QPair<double, double> originPosPercent;

    QString textLabel;
    QFontMetrics fm(d_ptr->font);
    /* clear the labels cache */
    d_ptr->labelsCacheHash.clear();

    ScaleItem *associatedAxis = d_ptr->view->associatedAxis(this->axisId());
    if(associatedAxis)
        originPosPercent = d_ptr->view->associatedOriginPosPercentage(associatedAxis->axisId(), d_ptr->axisId, &ok);

    if(d_ptr->orientation == Horizontal)
        originPercent = originPosPercent.first;
    else
        originPercent = originPosPercent.second;

    if(associatedAxis && ok)
        x0 = x1 + (x2 - x1) * originPercent;

    x = x0;
    while(x <= x2)
    {
        if(d_ptr->scaleLabelInterface)
            textLabel = d_ptr->scaleLabelInterface->label(x);
        else /* no, just return the number */
            textLabel.sprintf(qstoc(d_ptr->actualLabelsFormat), x);
        /* add item to cache */
        d_ptr->labelsCacheHash.insert(x, textLabel);

        width = fm.width(textLabel);
        if(max < width)
        {
            max = width;
            d_ptr->longestLabel = textLabel;
        }
        x += tickDist;
    }

    x = x0 - tickDist;
    while(x >= x1)
    {
        if(d_ptr->scaleLabelInterface)
            textLabel = d_ptr->scaleLabelInterface->label(x);
        else /* no, just return the number */
            textLabel.sprintf(qstoc(d_ptr->actualLabelsFormat), x);
        /* add item to cache */
        d_ptr->labelsCacheHash.insert(x, textLabel);

        width = fm.width(textLabel);
        if(max < width)
        {
            max = width;
            d_ptr->longestLabel = textLabel;
        }
        x -= tickDist;
    }
    d_ptr->maxLabelWidth = max;
}

void ScaleItem::redraw()
{
    d_ptr->mNeedFullRedraw = true;
    update();
}

/* recalculates the step len.
 * This method needs to be called when upper or lower bound changes or
 * when the plot zoom level changes.
 */
double ScaleItem::updateStepLen()
{
    double sLen;
    if(d_ptr->tickStepLen != -1)
    {

        sLen = d_ptr->tickStepLen;
    }
    else
    {
        double span = d_ptr->upperBound - d_ptr->lowerBound;
        double scale, factor, x;
        switch(d_ptr->orientation)
        {
        case ScaleItem::Horizontal:
            scale = d_ptr->view->QGraphicsView::transform().m11();
            break;
        default:
            scale = d_ptr->view->QGraphicsView::transform().m22();
            break;
        }

        span /= scale;
        double magnitude = floor(log10(span));
        factor = span / pow(10,magnitude);

        if(factor  < 2.5)
            x = 1;
        else if(factor < 5)
            x = 2;
        else
            x = 5;

        sLen = x * pow(10, (magnitude - 1));
    }

    if(d_ptr->actualTickStepLen != sLen)
    {
        d_ptr->actualTickStepLen = sLen;
        foreach(AxisChangeListener* l, d_ptr->axisChangeListeners)
        {
            l->tickStepLenChanged(sLen);
        }
    }

    return sLen;
}

void ScaleItem::setBoundsFromCurves()
{
    d_ptr->minMaxUnset = true;
    QList<SceneCurve *> curves = d_ptr->view->curvesForAxes(d_ptr->axisId, d_ptr->orientation);
    if(!curves.size())
        return;
    int i;
    double min = 0.0, max = 0.0, span;
    SceneCurve *c;
    Data *d ;
    unsigned int visibleCurvesCnt = 0;
    switch(d_ptr->orientation)
    {
    case Horizontal:
        for(i = 0; i < curves.size(); i++)
        {
            c = curves[i];
            if(c->curveItem() && c->curveItem()->isVisible())
            {
                d = c->data();
                if(visibleCurvesCnt == 0 || d->xMin < min)
                    min = d->xMin;
                if(visibleCurvesCnt == 0 || d->xMax > max)
                    max = d->xMax;
                visibleCurvesCnt++;
            }
        }
        break;
    default:
        for(i = 0; i < curves.size(); i++)
        {
            c = curves[i];
            if(c->curveItem() && c->curveItem()->isVisible())
            {
                d = c->data();
                if(visibleCurvesCnt == 0 || d->yMin < min)
                    min = d->yMin;
                if(visibleCurvesCnt == 0 || d->yMax > max)
                    max = d->yMax;
                visibleCurvesCnt++;
            }
        }
        break;
    }
    if(max >= min)
    {
        /* apply scale span adjustment (default is 2 % ) */
        span = max - min;
        max += span * d_ptr->autoscaleMargin / 2;
        min -= span * d_ptr->autoscaleMargin / 2;

        if(max == min) /* only one value in the curve */
        {
            if(max != 0)
            {
                max += min * 0.03;
                min -= min * 0.04;
            }
            else
            {
                max = 1;
                min = -1;
            }
        }
        if(d_ptr->lowerBound != min || d_ptr->upperBound != max)
        {
            d_ptr->lowerBound = min;
            d_ptr->upperBound = max;

            /* needs to be called before mUpdateLabelsFormat */
            updateStepLen();

            //            if(d_ptr->orientation == ScaleItem::Vertical)
            //                printf("\e[1;36msetyBoundsFromCurves (%s) ticl ke %f\e[0m\n",
            //                       qstoc(objectName()), d_ptr->actualTickStepLen);

            /* once the tick step len is up to date, update labels format */
            if(d_ptr->axisLabelsFormat.isEmpty()) /* avoid a function call if not needed */
                updateLabelsFormat(d_ptr->axisLabelsFormat);

            updateLabelsCache();

            foreach(AxisChangeListener* l, d_ptr->axisChangeListeners)
            {
                if(d_ptr->orientation == Horizontal)
                    l->xAxisBoundsChanged(min, max);
                else
                    l->yAxisBoundsChanged(min, max);
            }
            emit upperBoundChanged(max);
            emit lowerBoundChanged(min);
        }
    }
    else
        perr("ScaleItem::setBoundsFromCurves: max %f < min %f", max, min);
}

void ScaleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget *  )
{
    QPen axisPen(d_ptr->axisColor), gridPen(d_ptr->gridColor);
    axisPen.setWidthF(0.0);
    gridPen.setWidthF(0.0);


    QGraphicsView *view = NULL;
    if(scene()->views().size() > 0)
        view = scene()->views().first();
    QTransform tran = view->transform();


    painter->setClipRect(option->exposedRect.toRect());
    //    if(d_ptr->orientation == ScaleItem::Vertical)
    //        printf("\e[1;36mpaint [vertical](%s) z value %f\e[0m\n", qstoc(objectName()), zValue());
    double x1, x2, y1, y2, x, y, x0, y0;
    qreal px,  py, px0, py0;
    /* initialize canvas rect, width and height to plot rect */

    bool canvasRectChanged = false;
    QRectF rect = d_ptr->plotRect;
    QRectF scaledRect = tran.mapRect(rect);

    canvasWidth = rect.width();
    canvasHeight = rect.height();
    qreal mappedRectLeft = scaledRect.left();
    qreal mappedRectTop = scaledRect.top();
    qreal mappedRectRight = mappedRectLeft + scaledRect.width();
    qreal mappedRectBottom = mappedRectTop + scaledRect.height();


    //  painter->fillRect(tran.mapRect(rect), QBrush(Qt::blue));
    //    painter->setPen(Qt::red);
    //    painter->drawEllipse(transformedRect.center(), 10, 10);
    //    painter->drawEllipse(transformedRect.topLeft(), 10, 10);
    //    painter->drawEllipse(transformedRect.bottomRight(), 10, 10);

    //    painter->setPen(Qt::green);
    //    painter->drawRect(d_ptr->plotRect);

    //    qDebug() << __FUNCTION__ << "plot rect: " << d_ptr->plotRect << "rect" << rect
    //             << " transformeth " << tran.mapRect(rect)
    //             << "m11" << tran.m11() << " m22 " << tran.m22() << tran.m21() << tran.m21();

    qreal labelPos;
    qreal yZoomLabel = 0, xZoomLabel = 0;
    qreal labelHeight;
    bool axisOutsideViewport = false;
    qreal xMaxLabelSpace = 0.0, yMaxLabelSpace = 0.0;
    qreal tickStepLen;
    bool inZoom = d_ptr->view->inZoom();
    QString textLabel;
    QRectF txtRect(0, 0, 0, 0);
    painter->setFont(d_ptr->font);
    QFontMetrics fm(painter->font());
    labelHeight = fm.height();

    bool ok;
    QPair<double, double> originPosPercent;
    ScaleItem *associatedAxis = d_ptr->view->associatedAxis(this->axisId());
    if(associatedAxis)
        originPosPercent = d_ptr->view->associatedOriginPosPercentage(associatedAxis->axisId(), d_ptr->axisId, &ok);

    // printf("\e[1;36m origin of %d: %f\e[0m\n", axisId(), originPosPercent);

    if(!associatedAxis || !ok)
    {
        perr("ScaleItem::paint: no other axis is associated to \"%s\" (id %d)"
             " or origin undefined between the current and associated axis", qstoc(objectName()),
             axisId());
        return;
    }

    /* setBoundsFromCurves updates d_ptr->actualTickStepLen */
    if(d_ptr->autoScale && !inZoom)
        setBoundsFromCurves();
    /* else:
     * no need to recalculate tick step len at each paint event, because tick step length
     * only changes in one of the following cases:
     * - bounds change
     * - user sets a custom tick step len
     * - plot zoom level changes
     *
     * In all these cases, the tick step len is recalculated and stored in d_ptr->actualTickStepLen
     */

    tickStepLen = d_ptr->actualTickStepLen;

    /* if actual x and/or y labels format is not initialized, calculate it */
    if(d_ptr->actualLabelsFormat.isEmpty())
        updateLabelsFormat(d_ptr->axisLabelsFormat);

    switch(d_ptr->orientation)
    {
    case ScaleItem::Horizontal:
        x1 = d_ptr->lowerBound;
        x2 = d_ptr->upperBound;
        y1 = associatedAxis->lowerBound();
        y2 = associatedAxis->upperBound();

        xMaxLabelSpace = d_ptr->maxLabelWidth + d_ptr->tickWidth + d_ptr->labelMargin;
        yMaxLabelSpace = associatedAxis->maxLabelWidth() + d_ptr->tickWidth/2.0;
        break;

    default:
        x1 = associatedAxis->lowerBound();
        x2 = associatedAxis->upperBound();
        y1 = d_ptr->lowerBound;
        y2 = d_ptr->upperBound;/* x tick len: if set to -1 then automatically choose 20 steps */

        xMaxLabelSpace = associatedAxis->maxLabelWidth() + d_ptr->tickWidth + d_ptr->labelMargin;
        yMaxLabelSpace = d_ptr->maxLabelWidth + d_ptr->tickWidth/2.0;
        break;
    }

    if(x1 == x2 || y1 == y2)
        return;

    x0 = x1 + (x2 - x1) * originPosPercent.first;
    y0 = y1 + (y2 - y1) * originPosPercent.second;

    px0 = (scaledRect.width() - 1) * (x0 - x1) / (x2 - x1) + mappedRectLeft;
    py0 = scaledRect.height() - 1 - ((scaledRect.height() - 1) * (y0 - y1) / (y2 - y1) + mappedRectTop);

    if(px0 - yMaxLabelSpace < mappedRectLeft ||
            ((d_ptr->orientation == Vertical && d_ptr->axisLabelsOutsideCanvas)
             || (d_ptr->orientation == Horizontal && associatedAxis->axisLabelsOutsideCanvas())))

    {
        //        printf("\e[1;31m scene should be scaled axis lab %f scene left %f  px0 %f rect left %f\e[0m\n",
        //               px0 - yMaxLabelWidth, d_ptr->canvasRect.left(), px0, txtRect.left());

        //        qDebug() << " plot rect efor " << plotRect;
        mappedRectLeft = yMaxLabelSpace;
        rect.setLeft(yMaxLabelSpace / tran.m22());
        scaledRect.setLeft(mappedRectLeft);
        canvasRectChanged = true;
        //     qDebug() << "plotRectAdter " << plotRect;
    }
    if(py0 + xMaxLabelSpace > mappedRectBottom ||
            ( (d_ptr->orientation == Horizontal && d_ptr->axisLabelsOutsideCanvas)
              || (d_ptr->orientation == Vertical && associatedAxis->axisLabelsOutsideCanvas()) ) )
    {
        mappedRectBottom = mappedRectBottom - xMaxLabelSpace;
        scaledRect.setBottom(mappedRectBottom);
        //        qDebug() << __FUNCTION__ << "changing rect bottom of " << xMaxLabelSpace << tran.m11() << tran.m12() << tran.m21()
        //                 << tran.dx()
        //                 << (py0 + xMaxLabelSpace > mappedRectBottom);
        qreal mappedXLabelSpace = xMaxLabelSpace / tran.m11();
        rect.setBottom(rect.bottom() - mappedXLabelSpace);
        canvasRectChanged = true;
    }

    /* if axis labels have required a change in the space reserved to the curves
     * (canvas rect), then the canvas rect must be recalculated.
     * Let's use the inverted transform to calculate it.
     */
    if(canvasRectChanged || d_ptr->canvasRect != rect)
    {
        if(d_ptr->canvasRect != rect)
        {
            foreach(AxisChangeListener* l, d_ptr->axisChangeListeners)
                l->canvasRectChanged(rect);
            d_ptr->canvasRect = rect;
        }
        //        else
        //            printf("\e[1;31mno worries, canvas rect not changed\e[0m\n");
        canvasWidth = rect.width();
        canvasHeight = rect.height();
    }

    double prevx;
    switch(d_ptr->orientation)
    {
    case ScaleItem::Horizontal:

        py0 = scaledRect.height() - 1 - ((scaledRect.height() - 1) * (y0 - y1) / (y2 - y1) + mappedRectTop);

        if(d_ptr->axisLabelsOutsideCanvas)
            labelPos = mappedRectBottom - py0 + d_ptr->tickWidth/2.0 + d_ptr->labelMargin;
        else
            labelPos = d_ptr->axisLabelDist;
        /// printf("\e[1;32mdraw() scale item\e[0m \"%s\" y %.2f\n", qstoc(objectName()), py);

        ///        if(inZoom)
        //        {
        //            yLab1 = labelPos + py0 - fm.width(d_ptr->longestLabel);
        //            yLab2 = labelPos + py0 + fm.width(d_ptr->longestLabel);
        //            yZoom1 = viewportR.bottom() - labelHeight;
        //            yZoom2 = viewportR.top();

        //            if(yLab2 < yZoom2)
        //                yZoomLabel = yZoom2;
        //            else if(yLab1 > yZoom1)
        //                yZoomLabel = yZoom1;

        //            axisOutsideViewport = (yLab1 > yZoom1 || yLab2 < yZoom2);
        ///        }
        painter->setPen(axisPen);
        painter->drawLine(mappedRectLeft, py0 , mappedRectRight, py0);

        /* draw ticks starting from origin */
        d_ptr->mLastTickPos = (scaledRect.height() - 1) * (x0 - x1) / (x2 - x1) + mappedRectLeft;

        x = x0;

        while(x <= x2)
        {
            px = (scaledRect.width() - 1) * (x - x1) / (x2 - x1) + mappedRectLeft;

            //            printf("\e[1;32m dist from prev %.10f\e[0m, ", px - prevx);
            prevx = px;
            if(d_ptr->gridEnabled && x != x0)/* x != x0 not to draw grid over axes */
            {
                painter->setPen(gridPen);
                painter->drawLine(px, mappedRectTop, px, mappedRectBottom);
            }
            if(d_ptr->labelsEnabled)
            {
                /* ScaleLabelInterface installed ? */
                textLabel = d_ptr->labelsCacheHash.value(x);
                if(px - fm.height() > d_ptr->mLastTickPos || x == x0)
                {
                    painter->setPen(axisPen);
                    txtRect.setRect(0, 0, d_ptr->maxLabelWidth, labelHeight);
                    painter->translate(px + labelHeight/2, labelPos + py0);
                    painter->rotate(d_ptr->axisLabelRotation);
                    painter->drawText(txtRect, textLabel);
                    painter->rotate(-d_ptr->axisLabelRotation);
                    painter->translate(-px - labelHeight/2, -labelPos - py0);
                    d_ptr->mLastTickPos = px;
                }
                if(axisOutsideViewport) /* draw the labels on the border */
                {
                    painter->setPen(d_ptr->gridColor.darker());
                    painter->translate(px + labelHeight/2, yZoomLabel);
                    painter->rotate(d_ptr->axisLabelRotation);
                    txtRect.setRect(0, 0, d_ptr->maxLabelWidth, labelHeight);
                    painter->drawText(txtRect, textLabel);
                    painter->rotate(-d_ptr->axisLabelRotation);
                    painter->translate(-px - labelHeight/2, -yZoomLabel);
                }
            }
            painter->setPen(axisPen);
            painter->drawLine(px, py0 - d_ptr->tickWidth/2.0, px, py0 + d_ptr->tickWidth/2.0);
            x = x + tickStepLen;
        }
        //        printf("\e[0m\n");

        /* draw ticks from origin backwards */
        d_ptr->mLastTickPos = (scaledRect.width() - 1) * (x0 - x1) / (x2 - x1) + mappedRectLeft;
        x = x0 - tickStepLen;
        prevx = x1;
        while( x >= x1)
        {
            px = (scaledRect.width() - 1) * (x - x1)/ (x2 - x1) + mappedRectLeft;

            //            printf("\e[0;32m dist from prev %f\e[0m, ",  prevx - px);
            prevx = px;
            /* 1. draw grid */
            if(d_ptr->gridEnabled)
            {
                painter->setPen(gridPen);
                painter->drawLine(px, mappedRectTop, px, mappedRectBottom);
            }

            /* 2. draw labels */
            if(d_ptr->labelsEnabled)
            {
                textLabel = d_ptr->labelsCacheHash.value(x);
                if(px + fm.height() < d_ptr->mLastTickPos)
                {
                    painter->setPen(axisPen);
                    txtRect.setRect(0, 0, fm.width(textLabel), labelHeight);
                    painter->translate(px + labelHeight/2.0, py0 + labelPos);
                    painter->rotate(d_ptr->axisLabelRotation);
                    painter->drawText(txtRect, textLabel);
                    painter->rotate(-d_ptr->axisLabelRotation);
                    painter->translate(-px - labelHeight/2.0, -py0 - labelPos);
                    d_ptr->mLastTickPos = px;
                }
                if(axisOutsideViewport) /* draw the labels on the border */
                {
                    painter->setPen(d_ptr->gridColor.darker());
                    painter->translate(px + labelHeight/2.0, yZoomLabel);
                    txtRect.setRect(0, 0, d_ptr->maxLabelWidth, labelHeight);
                    painter->rotate(d_ptr->axisLabelRotation);
                    painter->drawText(txtRect, textLabel);
                    painter->rotate(-d_ptr->axisLabelRotation);
                    painter->translate(-px - labelHeight/2.0, -yZoomLabel);
                }
            }
            /* 3. draw ticks */
            painter->setPen(axisPen);
            painter->drawLine(px, py0 -d_ptr->tickWidth/2.0, px, py0 + d_ptr->tickWidth/2.0);

            x = x - tickStepLen;
        }

        /* Draw axis title */
        if(!d_ptr->axisTitle.isEmpty())
        {
            painter->setFont(d_ptr->axisTitleFont);
            painter->setPen(QPen(d_ptr->axisTitleColor));
            painter->drawText(mappedRectRight - d_ptr->mAxisTitleWidth - 4, py0 - d_ptr->tickWidth/2 - 1, d_ptr->axisTitle);
        }
        //        printf("\e[0m\n");

        // painter->setPen(Qt::green);
        //   painter->drawRect(boundingRect());

        break;

    case Vertical:
    default:
        y = y1;
        px0 = (scaledRect.width() - 1) * (x0 - x1) / (x2 - x1) + mappedRectLeft;

        if(d_ptr->axisLabelsOutsideCanvas)
            labelPos = -px0 - d_ptr->maxLabelWidth +mappedRectLeft - d_ptr->tickWidth/2.0; /* margin */
        else
            labelPos =  -d_ptr->axisLabelDist- d_ptr->maxLabelWidth;

        ///        if(inZoom)
        //        {
        //            xZoom1 = viewportR.left();
        //            xZoom2 = viewportR.right();
        //            xLab1 = px0 + labelPos + fm.width(d_ptr->longestLabel);
        //            xLab2 = px0 + labelPos/* - fm.width(d_ptr->longestLabel)*/;
        //            if(xLab2 > xZoom2)
        //                xZoomLabel = xZoom2 - fm.width(d_ptr->longestLabel);
        //            else if(xLab1 < xZoom1)
        //                xZoomLabel = xZoom1;
        //            axisOutsideViewport = (xLab1 < xZoom1 || xLab2 > xZoom2);
        ///        }

        painter->setPen(axisPen);
        painter->drawLine(px0, mappedRectTop , px0, mappedRectBottom);

        d_ptr->mLastTickPos = scaledRect.height() - 1 - ((scaledRect.height() - 1) * (y0 - y1) / (y2 - y1) + mappedRectTop);

        /* draw ticks starting from origin */
        y = y0;
        while(y <= y2)
        {
            py = (scaledRect.height() - 1) - ((scaledRect.height() - 1) * (y - y1) / (y2 - y1) + mappedRectTop);

            if(d_ptr->gridEnabled && y != y0) /* y != y0 not to draw grid over axes */
            {
                painter->setPen(gridPen);
                painter->drawLine(mappedRectLeft, py, mappedRectRight, py);
            }

            painter->setPen(axisPen);
            painter->drawLine(px0 - d_ptr->tickWidth/2.0, py, px0 + d_ptr->tickWidth/2.0, py);

            if(d_ptr->labelsEnabled)
            {
                textLabel = d_ptr->labelsCacheHash.value(y);
                txtRect.setRect(labelPos, -labelHeight/2, d_ptr->maxLabelWidth, fm.height());

                /* less than when  y positive, due to inverted Qt coordinate system for y axis */
                if(py + fm.height() < d_ptr->mLastTickPos || y == y0)
                {
                    painter->translate(px0, py);
                    painter->rotate(d_ptr->axisLabelRotation);
                    painter->drawText(txtRect, textLabel);
                    painter->rotate(-d_ptr->axisLabelRotation);
                    painter->translate(-px0, -py);
                    d_ptr->mLastTickPos = py;
                }
            }
            if(axisOutsideViewport) /* draw the labels on the border */
            {
                painter->setPen(d_ptr->gridColor.darker());
                painter->translate(xZoomLabel, py);
                painter->rotate(d_ptr->axisLabelRotation);
                txtRect.setRect(0, 0, d_ptr->maxLabelWidth, labelHeight);
                painter->drawText(txtRect, textLabel);
                painter->rotate(-d_ptr->axisLabelRotation);
                painter->translate(-xZoomLabel, -py);
            }

            y = y + tickStepLen;
        }

        d_ptr->mLastTickPos = scaledRect.height() - 1 - ((scaledRect.height() - 1) * (y0 - y1) / (y2 - y1) + mappedRectTop);
        /* draw ticks from origin backwards */
        y = y0 - tickStepLen;
        while(y >= y1)
        {
            /* y axis direction is inverted with respect to Qt coordinates system.
             * So sceneR.height() - ....
             */
            py = scaledRect.height() - 1 - ((scaledRect.height() - 1) * (y - y1)/ (y2 - y1) + mappedRectTop);

            if(d_ptr->gridEnabled)
            {
                painter->setPen(gridPen);
                painter->drawLine(mappedRectLeft, py, mappedRectRight, py);
            }

            painter->setPen(axisPen);
            painter->drawLine(px0 - d_ptr->tickWidth/2.0, py, px0 + d_ptr->tickWidth/2.0, py);

            if(d_ptr->labelsEnabled)
            {
                textLabel = d_ptr->labelsCacheHash.value(y);
                txtRect.setRect(labelPos, -labelHeight/2, fm.width(textLabel), labelHeight);
                /* > due to inverted Qt coordinate system for y axis
                 */
                if(py - labelHeight > d_ptr->mLastTickPos)
                {
                    painter->translate(px0, py);
                    painter->rotate(d_ptr->axisLabelRotation);
                    painter->drawText(txtRect, textLabel);
                    painter->rotate(-d_ptr->axisLabelRotation);
                    painter->translate(-px0, -py);
                    d_ptr->mLastTickPos = py;
                }
            }
            if(axisOutsideViewport) /* draw the labels on the border */
            {
                painter->setPen(d_ptr->gridColor.darker());
                painter->translate(xZoomLabel, py);
                txtRect.setRect(0, 0, d_ptr->maxLabelWidth, labelHeight);
                painter->drawText(txtRect, textLabel);
                painter->translate(-xZoomLabel, -py);

            }
            y = y - tickStepLen;
        }

        /* Draw axis title */
        if(!d_ptr->axisTitle.isEmpty())
        {
            painter->setFont(d_ptr->axisTitleFont);
            QPen axisTitlePen(d_ptr->axisTitleColor);
            axisTitlePen.setWidthF(0.0);
            painter->setPen(axisTitlePen);
            painter->drawText(px0 + d_ptr->tickWidth/2.0 + 3, mappedRectTop + d_ptr->mAxisTitleHeight + 2,  d_ptr->axisTitle);
        }
        //        painter->setPen(Qt::blue);
        //                QPen p = painter->pen();
        //                QColor c(Qt::blue);
        //        c.setAlpha(80);
        //                p.setColor(c);
        //        p.setWidth(2);
        //                painter->setPen(p);
        //                painter->drawRect(rect);
        ////        painter->drawRect(boundingRect());

        break;

        /* draw the scene rect for debug purposes */
        // painter->drawRect(-pos().x(), -pos().y(), sceneR.width(), sceneR.height());
    }

    /// DEBUG STUFF
    ///
    /*
    QPen canvasPen(Qt::green);
    canvasPen.setWidthF(2.5);
    painter->setPen(canvasPen);
    painter->drawRect(d_ptr->canvasRect);
    painter->setPen(Qt::blue);
    painter->drawRect(tran.mapRect(d_ptr->canvasRect));
    QPen pp(Qt::cyan);
    pp.setStyle(Qt::DashLine);
    painter->setPen(pp);
    painter->drawRect(scaledRect);

    */
}

QRectF ScaleItem::boundingRect () const
{
    QGraphicsView *view = NULL;
    if(scene()->views().size() > 0)
        view = scene()->views().first();
    if(!view)
        return QRectF();

    QTransform tran = view->transform();


    if(d_ptr->mNeedFullRedraw ||d_ptr->gridEnabled )
    {
        if(d_ptr->mNeedFullRedraw)
            d_ptr->mNeedFullRedraw = false;
        //        if(d_ptr->orientation == ScaleItem::Vertical)
        //            printf("\e[1;32m returning sceneReact() \"%s\"\e[0m\n", qstoc(objectName()));
        return tran.mapRect(scene()->sceneRect());
    }
    //    else
    //        printf("\e[1;31m not returning scene rect!\e[0m\n");

    QRectF plotR = tran.mapRect(d_ptr->view->plotRect());
    qreal x = 0, w, y = 0, h;
    double x1 = d_ptr->lowerBound;
    double x2 = d_ptr->upperBound;
    double y1 = d_ptr->lowerBound;
    double y2 = d_ptr->upperBound;

    qreal px0, py0;
    bool ok;
    QPair<double, double> originPosPercent;
    ScaleItem *associatedAxis = d_ptr->view->associatedAxis(this->axisId());
    if(associatedAxis)
        originPosPercent = d_ptr->view->associatedOriginPosPercentage(associatedAxis->axisId(), d_ptr->axisId, &ok);

    if(!associatedAxis || !ok)
    {
        perr("ScaleItem::boundingRect: no other axis is associated to \"%s\" (id %d)"
             " or origin undefined between the current and associated axis", qstoc(objectName()),
             axisId());
        return plotR;
    }

    qreal x0 = x1 + (x2 - x1) * originPosPercent.first;
    qreal y0 = y1 + (y2 - y1) * originPosPercent.second;

    switch(d_ptr->orientation)
    {
    case ScaleItem::Horizontal:
        py0 = plotR.height() - (plotR.height() * (y0 - y1) / (y2 - y1) + plotR.top());
        w = plotR.width();
        h = d_ptr->tickWidth + 100;
        y = py0 - d_ptr->tickWidth;
        break;
    default:
        px0 = plotR.width() * (x0 - x1) / (x2 - x1) + plotR.left();
        h = plotR.height();
        w = d_ptr->tickWidth + 100;
        x = px0 -d_ptr->tickWidth;

        break;
    }
    QRectF bounding(x, y, w, h);
    //    qDebug() << "scaleItem" << objectName() << "boundingRect() " << bounding <<
    //                "toSceneRect:" << this->mapToScene(bounding).boundingRect();
    return bounding;
}
