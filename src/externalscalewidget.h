#ifndef EXTERNALSCALEWIDGET_H
#define EXTERNALSCALEWIDGET_H

#include <QWidget>
#include <axischangelistener.h>
#include <QPen>
#include <plotgeometryeventlistener.h>

class ScalePrivate;
class QScrollArea;
class ScaleLabelInterface;

/** \brief A widget to configure and draw a scale external to a PlotSceneWidget
 *         or any other scroll area  whose contents need a scale.
 *
 * \par Structure
 * The ExternalScaleWidget is a QWidget that draws a scale with ticks and numeric
 * values. It can be horizontal or vertical. Implements AxisChangeListener in order
 * to receive notifications when the maximum and minimum values of the scale change
 * and PlotEventListener in order to be notified when the PlotSceneWidget area changes,
 * for instance when the user scales the view to magnify a portion of the scene.
 * Nonetheless, it is not essential to install an ExternalScaleWidget if you intend to
 * use this class without a PlotSceneWidget. Actually, there are slots equivalent to the
 * methods implemented from the AxisChangeListener and PlotEventListener interfaces.
 * These slots are areaChanged and scrollChanged. The ExternalScaleWidget needs to know
 * when the area of the QScrollArea contents changes and when the value of the scrollbars
 * change, so that it can recalculate the values to display in the correct points.
 *
 * \par Note
 * You can use the HorizontalScaleWidget and VerticalScaleWidget convenience classes
 * to directly instantiate horizontal and vertical scales respectively.
 * The Qt Desiger plugin uses those classes.
 *
 * \par Align scales with the PlotSceneWidget or other scrolling areas
 * It is very important that the height of a vertical scale is equal to the height
 * of the widget it is associated to, and that the width of a horizontal scale is
 * equal to the widget's width. Using the Qt4 designer, just place and align the
 * widgets, and then use a grid layout. The QGridLayout will ensure that the widgets
 * are all scaled together.
 *
 *
 *
 * \par Example
 * Suppose you have an image from a camera and you want to use an external scale
 * to identify pixel positions inside the image. In this case the ExternalScaleWidget
 * is not used in association with a PlotSceneWidget and thus we use signal/slot
 * connections to tell the scale that the image area or the values of the
 * scroll bars have changed.
 *
 * \code
 * // define the orientation of each scale widget
 * ui.xScale->setOrientation(ScaleItem::Horizontal);
 * ui.yScale->setOrientation(ScaleItem::Vertical);
 * //
 * // set the scroll area associated to each scale
 * // ui.image_ScrollArea is a QScrollArea (or QAbstractScrollArea)
 * // ui.xScale and ui.yScale are the two ExternalScaleWidget
 * //
 * connect(ui.image_ScrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), ui.xScale,
 *       SLOT(scrollChanged(int)));
 * connect(ui.image_ScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), ui.yScale,
 *       SLOT(scrollChanged(int)));
 *
 * // if the size of the contents of the QScrollArea changes, then each scale must be
 * // notified
 * //
 * connect(myImage, SIGNAL(imageSizeChanged(QSizeF)), ui.xScale, SLOT(areaChanged(QSizeF)));
 * connect(myImage, SIGNAL(imageSizeChanged(QSizeF)), ui.yScale, SLOT(areaChanged(QSizeF)));
 * \endcode
 *
 * \par Inverted scales
 * The maximum must always be greater than the minimum, but if you want the scale to draw
 * the values starting with the minimum on the rigth (for a horizontal scale) or the top
 * (for a vertical scale), just call setInverted on the ExternalScaleWidget.
 *
 * \par Tick drawing inverted
 * If a vertical scale is placed on the right of the display area, then you may want to
 * call setTickDrawingInverted in order to place ticks before numeric values. The same
 * goes for <em>top</em> placed horizontal scales.
 *
 * \par Labels format
 * The format of the labels is not automatically calculated as in the ScaleItem.
 * It is <em>%.1f</em> by default, and you must call the setFormat or use the format
 * option in order to change it.
 *
 * \par Origin position percentage
 * A property called originPercentage is introduced in order to tune the scale positioning.
 * A value of 0.5 for instance tells the scale that its <em>center of gravity</em> should be
 * in the middle of its length. A tick is guaranteed to be positioned at the half of the
 * length of the scale and, according to the number of ticks, some extra empty space at
 * the beginning or end of the scales might appear, without ticks. Actually, if used in
 * conjunction with a PlotSceneWidget, the origin of its axes is normally placed at the
 * center of the plot. To align the ExternalScaleWidget with the grid of the PlotSceneWidget,
 * you should tune the originPercentage property so that its value is 0.5.
 * The PlotSceneWidget itself has a property to change the axis origin position percentage:
 * on the ExternalScaleWidget an equal value should be set with setOriginPercentage.
 *
 * \par Orientation
 * The horizontal property is a shortcut for setOrientation. It switches a scale from the
 * horizontal to the vertical layout, being the parameter true or false respectively.
 *
 * \par Tick length
 * The tickLen property assigns a length (in pixel coordinates) to the scale ticks.
 *
 * \par Tick <em>step</em> length
 * The tick step length is used to calculate the number of ticks on the scale and where
 * the ticks have to be placed. The value is expressed in <em>scale coordinates</em>.
 * For instance, if your scale spans from 0 to 100 and you specify a tickStepLen of 10,
 * you will have ten ticks with ten values drawn on the widget.
 *
 * \par Label Rotation
 * The labelRotation property is able to rotate the labels on the scale. Up to now, it
 * is only possible to rotate <em>x axis</em> labels.
 *
 * \par Notes on widget geometry.
 * Whenever one of the minimum, maximum, labelRotation, format or tickLen properties changes,
 * the minimumSizeHint for the widget changes, and the updateGeometry() is internally called
 * by the setter methods. In this way you can safely insert the widgets into a QLayout without
 * worrying about geometry changes (resizes, changes in tick len or format...). The minimumSizeHint
 * takes into account the font in use, naturally.
 *
 * \par Example
 * This example shows how to use two ExternalScaleWidget scales with a PlotSceneWidget.
 * As said before, ExternalScaleWidget implements two interfaces aimed at receive
 * PlotSceneWidget notifications in matter of geometry change and ScaleItem notifications
 * in matter of minimum and maximum value changes.
 * All you need to to is to install each ExternalScaleWidget on the plot and on the scale
 * items of the plots as in the example below.
 *
 * \code
 *
 * // In this example we have a PlotSceneWidget called horProfile and a couple
 * // of ExternalScaleWidget scales called xScale and yScale
 * // take each of the ScaleItem of the PlotSceneWidget and install the scale
 * // as AxisChangeListener.
 * //
 * horProfile->xScaleItem()->installAxisChangeListener(xScale);
 * horProfile->yScaleItem()->installAxisChangeListener(yScale);
 *
 * // Install the scale as PlotChangeListener into the PlotSceneWidget (horProfile)
 * //
 * horProfile->installPlotChangeListener(xScale);
 * horProfile->installPlotChangeListener(yScale);
 *
 * // You may want to have the same tickStepLen on the plot and on the external scale.
 * // Supposing the plot has a tickStepLen of 50 on the x axis and 500 on the y axis,
 * // then:
 * //
 * xScale->setTickStepLen(50.0);
 * yScale->setTickStepLen(500.0);
 *
 * // The PlotSceneWidget center its axes (and grid) at the center of the plot so, if
 * // you do not change this position, you have to tune the position of the scales
 * // as follows:
 * //
 *  yScale->setOriginPercentage(0.5);
 *  xScale->setOriginPercentage(0.5);
 *
 * // For the same reason as above, if the size of the plot is less than the view size,
 * // the scales must be kept correctly centered:
 * //
 * xScale->setAlignment(Qt::AlignHCenter);
 * yScale->setAlignment(Qt::AlignVCenter);
 *
 * \endcode
 *
 * \par Sample code
 * An example can be found inside the directory <em>examples/externalscales</em> directory.
 * \dontinclude examples/externalscales/externalscales.cpp
 * \dontinclude examples/externalscales/externalscales.h
 * \dontinclude examples/externalscales/main.cpp
 *
 *
 * \image html externalScales.png
 *
 * @see HorizontalScaleWidget
 * @see VerticalScaleWidget
 * @see PlotSceneWidget
 * @see ScaleItem
 *
 * \par Personalized labels
 * You can implement a ScaleLabelInterface and install it on an ExternalScaleWidget.
 * The ExternalScaleWidget will call the ScaleLabelInterface::label implementation
 * with the value corresponding to the tick being drawn. Your ScaleLabelInterface::label
 * implementation has to convert the raw value passed by the ExternalScaleWidget into
 * a convenient label according to your needs. For instance, a TimeScaleLabel can be
 * used to convert unix timestamps into a date/time string.
 *
 */
class ExternalScaleWidget :
        public QWidget,
        public AxisChangeListener,
        public PlotGeometryEventListener
{
    Q_OBJECT

    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(double tickStepLen READ tickStepLen WRITE setTickStepLen)
    Q_PROPERTY(double originPercentage READ originPercentage WRITE setOriginPercentage)
    Q_PROPERTY(double scaleFactor READ scaleFactor WRITE setScaleFactor)
    Q_PROPERTY(double labelRotation READ labelRotation WRITE setLabelRotation)
    Q_PROPERTY(int tickLen READ tickLen WRITE setTickLen)
    Q_PROPERTY(int labelDistFromTick READ labelDistFromTick WRITE setLabelDistFromTick)
    Q_PROPERTY(bool isInverted READ isInverted WRITE setInverted)
    Q_PROPERTY(bool horizontal READ horizontal WRITE setHorizontal)
    Q_PROPERTY(bool tickDrawingInverted READ tickDrawingInverted WRITE setTickDrawingInverted)
    Q_PROPERTY(QString format READ format WRITE setFormat)
    Q_PROPERTY(bool syncAxisTicksWithPlot READ syncAxisTicksWithPlot WRITE setSyncAxisTicksWithPlot)
    Q_PROPERTY(bool syncAxisLabelsWithPlot READ syncAxisLabelsWithPlot WRITE setSyncAxisLabelsWithPlot)
    Q_PROPERTY(double margin READ margin WRITE setMargin)

public:
    explicit ExternalScaleWidget(QWidget *parent, ScaleItem::Orientation o = ScaleItem::Horizontal);

    ScaleItem::Orientation orientation() const;

    void setOrientation(ScaleItem::Orientation o);

    double minimum() const;

    double maximum() const;

    double margin() const;

    bool isInverted() const;

    bool horizontal() const;

    int tickLen() const;

    int labelDistFromTick() const;

    double tickStepLen() const;

    double originPercentage() const;

    double scaleFactor() const;

    double labelRotation() const;

    bool syncAxisTicksWithPlot() const;

    bool syncAxisLabelsWithPlot() const;

    virtual void xAxisBoundsChanged(double lower, double upper);

    virtual void yAxisBoundsChanged(double lower, double upper);

    virtual void axisAutoscaleChanged(ScaleItem::Orientation o, bool autoscale);

    virtual void plotAreaChanged(const QRectF& newRect);

    virtual void tickStepLenChanged(double  len);

    virtual void labelsFormatChanged(const QString& format);

    /* v---- reimplemented from plot event listener ---v */

    virtual void plotRectChanged(const QRectF &newRect);

    virtual void plotAreaChanged(const QSizeF &area);

    virtual void scrollBarChanged(Qt::Orientation orientation, int value);

    virtual void plotZoomLevelChanged(int ) {}

    /* ^--- end of PlotEventListener method implementations */

    QString format() const;

    Qt::Alignment alignment() const;

    bool tickDrawingInverted() const;

    void installScaleLabelInterface(ScaleLabelInterface *);

    void removeScaleLabelInterface();
    
signals:
    
public slots:
    void setMinimum(double min);

    void setMaximum(double max);

    void scrollChanged(int value);

    void scrollRangeChanged(int min, int max);

    void areaChanged(const QSizeF &size);

    void setInverted(bool);

    void setTickLen(int len);

    void setLabelDistFromTick(int d);

    void setTickStepLen(double tickStepLen);

    void setAlignment(Qt::Alignment alignment);

    void setMargin(double margin);

    void setOriginPercentage(double percent);

    void setScaleFactor(double);

    void setHorizontal(bool h);

    void setFormat(const QString& fmt);

    void setTickDrawingInverted(bool inverted);

    void setLabelRotation(double r);

    void setSyncAxisTicksWithPlot(bool sync);

    void setSyncAxisLabelsWithPlot(bool sync);

    QPen pen() const;

    void setPen(const QPen& p);

protected:
    void paintEvent(QPaintEvent *);

    virtual QSize minimumSizeHint() const;

    int mCalculateLongestLabelWidth() const;

private:

    ScalePrivate* d_ptr;

    Q_DECLARE_PRIVATE(Scale)
    
};

#endif // SCALE_H
