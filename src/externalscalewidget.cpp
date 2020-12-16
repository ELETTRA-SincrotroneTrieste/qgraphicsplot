#include "externalscalewidget.h"
#include <QPaintEvent>
#include <QPainter>
#include "qgraphicsplotmacros.h"
#include "scalelabelinterface.h"
#include <QtDebug>
#include <math.h>
#include <QScrollBar>
#include <QLayout>

class ScalePrivate
{
public:
    double min, max, tickStepLen, labelRotation, margin;
    double len, pos, offset, originPercentage, originOffset, scaleFactor;
    ScaleItem::Orientation orientation;
    bool inverted, tickDrawingInverted;
    bool syncAxisTicksWithPlot;
    bool syncAxisLabelsWithPlot;
    int tickLen;
    QString format;
    int longestLabelWidth, labelDistFromTick;
    Qt::Alignment alignment;
    ScaleLabelInterface *scaleLabelInterface;
    QPen pen;
};

ExternalScaleWidget::ExternalScaleWidget(QWidget *parent, ScaleItem::Orientation orientation) :
    QWidget(parent)
{
    d_ptr = new ScalePrivate();
    d_ptr->orientation = orientation;
    d_ptr->min = -1000;
    d_ptr->max = 1000;
    d_ptr->tickStepLen = 200;
    d_ptr->len = 0;
    d_ptr->pos = 0;
    d_ptr->offset = 0;
    d_ptr->inverted = false;
    d_ptr->tickLen = 5;
    d_ptr->labelDistFromTick = 2;
    d_ptr->alignment = Qt::AlignLeft;
    d_ptr->originPercentage = 0.0;
    d_ptr->originOffset = 0.0;
    d_ptr->scaleFactor = 1.0;
    d_ptr->tickDrawingInverted = false;
    d_ptr->labelRotation = 0.0;
    d_ptr->longestLabelWidth = 0;
    d_ptr->scaleLabelInterface = NULL;
    d_ptr->syncAxisTicksWithPlot = true;
    d_ptr->syncAxisLabelsWithPlot = true;
    d_ptr->margin = 2;
    d_ptr->pen = QPen(QColor(Qt::black));

    /* initialize len so that the scale is finely drawn when
     * dropped into a Qt4 designer form.
     * Check the size with the plugin PlotSceneWidget rect:
     * it should be 450x300.
     */
    if(orientation == ScaleItem::Horizontal)
        d_ptr->len = 450;
    else
        d_ptr->len = 300;

    update();
}

void ExternalScaleWidget::xAxisBoundsChanged(double lower, double upper)
{
    if(d_ptr->orientation == ScaleItem::Horizontal)
    {
        d_ptr->min = lower;
        d_ptr->max = upper;
        d_ptr->longestLabelWidth = mCalculateLongestLabelWidth();
        updateGeometry();
        update();
    }
}

void ExternalScaleWidget::yAxisBoundsChanged(double lower, double upper)
{
    if(d_ptr->orientation == ScaleItem::Vertical)
    {
        d_ptr->min = lower;
        d_ptr->max = upper;
        d_ptr->longestLabelWidth = mCalculateLongestLabelWidth();
        updateGeometry();
        update();
    }
}

void ExternalScaleWidget::axisAutoscaleChanged(ScaleItem::Orientation, bool )
{
    /* nothing to do here */
}

void ExternalScaleWidget::tickStepLenChanged(double  len)
{
    if(d_ptr->syncAxisTicksWithPlot)
        setTickStepLen(len);
}

void ExternalScaleWidget::labelsFormatChanged(const QString& format)
{
    if(d_ptr->syncAxisLabelsWithPlot)
        setFormat(format);
}

void ExternalScaleWidget::canvasRectChanged(const QRectF& )
{

}

/** \brief does nothing. We are not actually interested in the plot rect
 *
 */
void ExternalScaleWidget::plotRectChanged(const QRectF &r)
{
    qDebug() << "plotRectChanged" << r;
}

void ExternalScaleWidget::plotAreaChanged(const QSizeF &area)
{
    this->areaChanged(area);
}

void ExternalScaleWidget::scrollBarChanged(Qt::Orientation orientation, int value)
{
    if(orientation == Qt::Horizontal && d_ptr->orientation == ScaleItem::Horizontal)
        this->scrollChanged(value);
    else if(orientation == Qt::Vertical && d_ptr->orientation == ScaleItem::Vertical)
        this->scrollChanged(value);
}

void ExternalScaleWidget::setScaleFactor(double f)
{
    d_ptr->scaleFactor = f;
}

double ExternalScaleWidget::scaleFactor() const
{
    return d_ptr->scaleFactor;
}

void ExternalScaleWidget::setOriginPercentage(double percent)
{
    d_ptr->originPercentage = percent;
    update();
}

double ExternalScaleWidget::originPercentage() const
{
    return d_ptr->originPercentage;
}

ScaleItem::Orientation ExternalScaleWidget::orientation() const
{
    return d_ptr->orientation;
}

void ExternalScaleWidget::setOrientation(ScaleItem::Orientation o)
{
    d_ptr->orientation = o;
    update();
}

void ExternalScaleWidget::setInverted(bool inv)
{
    d_ptr->inverted = inv;
    update();
}

void ExternalScaleWidget::setFormat(const QString &fmt)
{
    d_ptr->format = fmt;
    d_ptr->longestLabelWidth = mCalculateLongestLabelWidth();
    updateGeometry();
    update();
}

QString ExternalScaleWidget::format() const
{
    return d_ptr->format;
}

void ExternalScaleWidget::setAlignment(Qt::Alignment alignment)
{
    d_ptr->alignment = alignment;
}

Qt::Alignment ExternalScaleWidget::alignment() const
{
    return d_ptr->alignment;
}

bool ExternalScaleWidget::isInverted() const
{
    return d_ptr->inverted;
}

void ExternalScaleWidget::setTickLen(int len)
{
    d_ptr->tickLen = len;
    updateGeometry();
}

int ExternalScaleWidget::tickLen() const
{
    return d_ptr->tickLen;
}

double ExternalScaleWidget::tickStepLen() const
{
    return d_ptr->tickStepLen;
}

void ExternalScaleWidget::setTickStepLen(double tickStepLen)
{
    if(tickStepLen > 0)
    {
        d_ptr->tickStepLen = tickStepLen;
        updateGeometry();
    }
    else
        perr("ExternalScaleWidget::setTickStepLen(): tickStepLen <=0 unsupported!");
}

void  ExternalScaleWidget::setLabelDistFromTick(int d)
{
    d_ptr->labelDistFromTick = d;
    updateGeometry();
}

double ExternalScaleWidget::margin() const
{
    return d_ptr->margin;
}

void ExternalScaleWidget::setMargin(double margin)
{
    d_ptr->margin = margin;
}

int ExternalScaleWidget::labelDistFromTick() const
{
    return d_ptr->labelDistFromTick;
}

void ExternalScaleWidget::setTickDrawingInverted(bool inverted)
{
    d_ptr->tickDrawingInverted = inverted;
    update();
}

bool ExternalScaleWidget::tickDrawingInverted() const
{
    return d_ptr->tickDrawingInverted;
}

void ExternalScaleWidget::setMinimum(double min)
{
    d_ptr->min = min;
    updateGeometry();
}

void ExternalScaleWidget::setMaximum(double max)
{
    d_ptr->max = max;
    updateGeometry();
}

double ExternalScaleWidget::minimum() const
{
    return d_ptr->min;
}

double ExternalScaleWidget::maximum() const
{
    return d_ptr->max;
}

void ExternalScaleWidget::setLabelRotation(double r)
{
    d_ptr->labelRotation = r;
    updateGeometry();
    update();
}

double ExternalScaleWidget::labelRotation() const
{
    return d_ptr->labelRotation;
}

bool ExternalScaleWidget::syncAxisLabelsWithPlot() const
{
    return d_ptr->syncAxisLabelsWithPlot;
}

bool ExternalScaleWidget::syncAxisTicksWithPlot() const
{
    return d_ptr->syncAxisTicksWithPlot;
}

void ExternalScaleWidget::setSyncAxisTicksWithPlot(bool sync)
{
    d_ptr->syncAxisTicksWithPlot = sync;
}

void ExternalScaleWidget::setSyncAxisLabelsWithPlot(bool sync)
{
    d_ptr->syncAxisLabelsWithPlot = sync;
}

QPen ExternalScaleWidget::pen() const
{
    return d_ptr->pen;
}

void ExternalScaleWidget::setPen(const QPen &p)
{
    d_ptr->pen = p;
}

void ExternalScaleWidget::setHorizontal(bool h)
{
    h ? setOrientation(ScaleItem::Horizontal) : setOrientation(ScaleItem::Vertical);
}

bool ExternalScaleWidget::horizontal() const
{
    return d_ptr->orientation == ScaleItem::Horizontal;
}

/** \brief When the scale is drawn, the value (in double format) is passed to
 *         the installed implementation of ScaleLabelInterface. The returned
 *         QString is used to draw the label corresponding to the value.
 *
 * When the scale is drawn, the value (in double format) is passed to
 *         the installed implementation of ScaleLabelInterface. The returned
 *         QString is used to draw the label corresponding to the value.
 *
 * @param iface an implementation instance of the ScaleLabelInterface
 * @see TimeScaleLabel
 *
 */
void ExternalScaleWidget::installScaleLabelInterface(ScaleLabelInterface *iface)
{
    d_ptr->scaleLabelInterface = iface;
}

void ExternalScaleWidget::removeScaleLabelInterface()
{
    d_ptr->scaleLabelInterface = NULL;
}

void ExternalScaleWidget::scrollChanged(int value)
{
    d_ptr->offset = value;
    update();
}

void ExternalScaleWidget::scrollRangeChanged(int min, int max)
{
    Q_UNUSED(min);
    Q_UNUSED(max);
}

void ExternalScaleWidget::areaChanged(const QSizeF& size)
{
     qDebug() << __FUNCTION__ << objectName() << size;
    switch(d_ptr->orientation)
    {
    case ScaleItem::Horizontal:
        d_ptr->len = size.width();
        break;
    case ScaleItem::Vertical:
        d_ptr->len = size.height();
        break;
    }
    update();
}

/** version with originPercentage
 *
 */
void ExternalScaleWidget::paintEvent(QPaintEvent *)
{
    if(d_ptr->len <= 0)
        return;

    QRect rect = geometry();
    double pixstep, offset, valueoffset;
    double offset_align = 0.0;
    double origin_offset = 0;
    double origin;
    double txth, txtx, x, y, val, tick;
    double nTicks = (d_ptr->max - d_ptr->min) / d_ptr->tickStepLen;
    QString valueStr;
    QString format;
    double w = width() * d_ptr->scaleFactor;
    double h = height() * d_ptr->scaleFactor;
    if(!d_ptr->format.isEmpty())
        format = d_ptr->format;
    else
        format = "%.1f";

    pixstep = (d_ptr->len - 1) / nTicks;
    if(pixstep < 4)
        pixstep = 4;
//    if(d_ptr->orientation == ScaleItem::Horizontal)
//        printf("\e[1;32m max min %f / %f\e[0m\n", d_ptr->max, d_ptr->min);
    /* d_ptr->offset contains the scroll bar value
     * d_ptr->len contains the length of the plot.
     */
    valueoffset = (d_ptr->max - d_ptr->min) * d_ptr->offset / (d_ptr->len);

    y = 0;
    int i;

    QPainter p(this);
    p.setPen(d_ptr->pen);
    QFont f = p.font();
    QFontMetrics fm(f);
    int fontHeight = fm.height();

    origin = (d_ptr->len - 1) * d_ptr->originPercentage;
    /* qRound(origin) % qRound(pixstep) */
    origin_offset = origin - (floor(origin/pixstep) * pixstep);

   // if(d_ptr->offset == 0)
     offset_align = d_ptr->margin;

    switch(d_ptr->orientation)
    {
    case ScaleItem::Horizontal:

        if(w > d_ptr->len)
        {
            switch(d_ptr->alignment)
            {
            /* -1 is because if the available rect has length 100,
             * we use pixels 0 to 99 and so we are able to draw
             * up to the last available pixel.
             * This is the approach used in ScaleItem, so it is
             * essential to maintain this behaviour in order to
             * exactly place ticks and grid on canvas aligned to
             * ticks of the external scale
             */
            case Qt::AlignHCenter:
                if(w > d_ptr->len + 1)
                    offset_align = ((w - d_ptr->len) / 2.0 - 1) / d_ptr->scaleFactor;
      //          qDebug() << __FUNCTION__ << "w: " << w << ", d_ptr->len" << d_ptr->len << ": offset align" << offset_align;
                break;
            case Qt::AlignRight:
                offset_align = w  - d_ptr->len - 1;
                break;
            default:
                break;
            }
        }

        if(d_ptr->tickDrawingInverted)
        {
            txth = fontHeight;
            y = txth + 2;
        }
        else
        {
            txth = rect.height();
            y = 0;
        }

        offset = (w - 1)  * d_ptr->offset / (d_ptr->len);
       // x = qRound(offset) % qRound(pixstep);


        x = offset - (floor(offset/pixstep) * pixstep);
   //     printf("\e[0;36m x: %f - pixstep %f d_ptr->len %f\e[0m\n", x, pixstep, d_ptr->len);
        for(i = 0; i < nTicks; i++)
        {
            /* origin offset must be taken into account to calculate the value
             * associated to the tick. offset_align no.
             */
            tick = x + i * pixstep + origin_offset;

            p.drawLine(tick + offset_align, y, tick + offset_align, y + d_ptr->tickLen);

            /* inverted scale? */
            if(d_ptr->inverted)
                val = d_ptr->max - (valueoffset + tick * (d_ptr->max - d_ptr->min) / (d_ptr->len -1 ));
            else
                val = d_ptr->min + valueoffset + tick * (d_ptr->max - d_ptr->min) / (d_ptr->len - 1);

//            if(d_ptr->orientation == ScaleItem::Horizontal)
//                printf("\e[1;34mtick [%f]: %f (offset_align %f pixstep %f origin_off %f qRound(pixstep) %d offset %f)\e[0m\n",
//                   val, tick, offset_align, pixstep, origin_offset, qRound(pixstep), offset);

            /* custom labels for the value val ? */
            if(d_ptr->scaleLabelInterface)
                valueStr = d_ptr->scaleLabelInterface->label(val);
            else
                valueStr = QString().sprintf(qstoc(format), val);

            if(d_ptr->labelRotation != 0)
            {
                p.translate(tick + offset_align, 0);
                p.rotate(d_ptr->labelRotation);
//                p.drawRect(QRect(0, -fontHeight/2, d_ptr->longestLabelWidth, fontHeight));
                p.drawText(QRect(0, -fontHeight/2, d_ptr->longestLabelWidth, fontHeight), Qt::AlignRight, valueStr);
                p.rotate(-d_ptr->labelRotation);
                p.translate( -(tick + offset_align), -0);
            }
            else
                p.drawText(tick + offset_align, d_ptr->tickLen + d_ptr->labelDistFromTick + fm.height(), valueStr);


        }
        break;

    default: /* Vertical */

        if(h > d_ptr->len)
        {
            switch(d_ptr->alignment)
            {
            /* see comment in Horizontal case for the -1 subtraction
             */
            case Qt::AlignVCenter:
                if(h > d_ptr->len + 1)
                    offset_align = ((h - d_ptr->len) / 2.0 - 1) / d_ptr->scaleFactor;
                break;
            case Qt::AlignBottom:
                offset_align = h - d_ptr->len - 1;
                break;
            default:
                break;
            }
        }

        if(d_ptr->tickDrawingInverted)
        {
            txtx = d_ptr->tickLen + 1;
            y = 0; /* start of tick drawing on the x coordinate */
        }
        else
        {
            txtx = 0;
            y = d_ptr->longestLabelWidth + 2;
        }

//        printf("\e[1;36m offset_align %f percentage %f pixstep %f d_ptr->len %f\e[0m\n",
//               offset_align, d_ptr->originPercentage, pixstep, d_ptr->len);

        offset = (h - 1) * d_ptr->offset / d_ptr->len;
        // x = qRound(offset) % qRound(pixstep);

        x = offset - (floor(offset/pixstep) * pixstep);

        for(i = 0; i < nTicks; i++)
        {
            /* origin offset must be taken into account to calculate the value
             * associated to the tick. offset_align no.
             */
            tick = x + i * pixstep + origin_offset;
            p.drawLine(y, tick + offset_align,
                       y + d_ptr->tickLen, tick + offset_align);

            /* inverted scale? */
            if(!d_ptr->inverted)
                val = d_ptr->max - (valueoffset + tick * (d_ptr->max - d_ptr->min) / (d_ptr->len -1 ));
            else
                val = d_ptr->min + valueoffset + tick * (d_ptr->max - d_ptr->min) / (d_ptr->len -1 );

            /* custom labels for the value val ? */
            if(d_ptr->scaleLabelInterface)
                valueStr = d_ptr->scaleLabelInterface->label(val);
            else
                valueStr = QString().sprintf(qstoc(format), val);

            p.drawText(txtx + d_ptr->labelDistFromTick, tick + fontHeight/2 + offset_align, valueStr);
        }
        break;
    }
}

int ExternalScaleWidget::mCalculateLongestLabelWidth() const
{
    QFont f = this->font();
    QFontMetrics fm(f);
    if(d_ptr->scaleLabelInterface)
        return fm.width(d_ptr->scaleLabelInterface->longestLabel());

    int maxlabelwidth;
    /* suppose that, for a given format, max and min values, it is enough
     * to calculate the width of the label corresponding to the minimum,
     * then the one corresponding to the maximum, compare them, take the
     * longest and add some pixels, just in case...
     */
    QString s, format;
    if(d_ptr->format.isEmpty())
        format = "%.1f";
    else
        format = d_ptr->format;
    s = s.sprintf(qstoc(format), d_ptr->min);
    maxlabelwidth = fm.width(s);
    s = s.sprintf(qstoc(format), d_ptr->max);
    if(maxlabelwidth < fm.width(s))
        maxlabelwidth = fm.width(s);

    return maxlabelwidth + 3; /* little correction, just to be sure */
}

/** \brief returns the minimum size hint.
 *
 * Must be recalculated when the min and max change or the
 * format changes.
 */
QSize ExternalScaleWidget::minimumSizeHint() const
{
    QFont f = this->font();
    QFontMetrics fm(f);
    double height;
    /* update longest label width */
    int maxlabelwidth = mCalculateLongestLabelWidth();

    switch(d_ptr->orientation)
    {
    case ScaleItem::Horizontal:
        height = fm.height() + d_ptr->tickLen + d_ptr->labelDistFromTick + maxlabelwidth *
                (sin(d_ptr->labelRotation * M_PI / 180.0));
        qDebug() << __FUNCTION__ << "height " << height;
        return QSize(300, qRound(height));

    case ScaleItem::Vertical:
        return QSize(maxlabelwidth + d_ptr->tickLen +  d_ptr->labelDistFromTick + 2, 300);
    }
    return QWidget::minimumSizeHint();
}



