#ifndef SCALEITEM_H
#define SCALEITEM_H

#include <QGraphicsObject>
#include <plotgeometryeventlistener.h>
#include <QFont>
#include <QDateTime>

class ScaleItemPrivate;
class QGraphicsPlotItem;
class AxisChangeListener;
class ScaleLabelInterface;
class QDateTime;


/** \brief The QGraphicsObject that draws a plot scale on a PlotSceneWidget
  *
  * \par Description
  * The ScaleItem is a QGraphicsObject that draws and axis scale on a PlotSceneWidget.
  * What a ScaleItem draws is:
  * \li a line for the backbone;
  * \li ticks on the backbone
  * \li labels on each tick, when there is enough space to draw the text.
  *
  * \par Position of the axes
  * The axes are placed by PlotSceneWidget through the classes AxesManager and AxisCouple.
  * Each AxisCouple in the PlotSceneWidget represents a couple of axis, one with Horizontal
  * orientation and the other with Vertical orientation.
  * The default positioning of the axes is done by default such as each the axes cross each
  * other in the middle.
  * If you want to change the reciprocal axes position, see PlotSceneWidget::setOriginPosPercentage
  * PlotSceneWidget::setDefaultXAxisOriginPosPercentage and
  * PlotSceneWidget::setDefaultYAxisOriginPosPercentage.
  *
  * \par Relevant axis properties
  * \li tickStepLen the distance between two subsequent ticks, in scale coordinates. A tick
  *     step length of one on the x axis means one tick every increment of one on the x value
  *     If tickStepLen is set to -1, then the ticks distance is automatically chosen.
  * \li axisLabelsEnabled enables or disables the text labels on the axis.
  * \li gridEnabled shows or hides the grid on the plot
  * \li axisLabelsFormat the format of the number to display on the axis (it's printf() format)
  * \li upperBound sets the upper bound on the scale item. axisAutoscaleEnabled must be set to false
  * \li lowerBound sets the lower bound on the scale item. axisAutoscaleEnabled must be set to false
  * \li axisAutoscaleEnabled enables or disables auto scaling.
  *
  */
class ScaleItem : public QGraphicsObject, public PlotGeometryEventListener
{
Q_OBJECT
    Q_PROPERTY(double tickStepLen READ tickStepLen WRITE setTickStepLen)
    Q_PROPERTY(bool axisLabelsEnabled READ axisLabelsEnabled WRITE setAxisLabelsEnabled)
    Q_PROPERTY(bool gridEnabled READ gridEnabled WRITE setGridEnabled)
    Q_PROPERTY(double axisLabelsRotation READ axisLabelsRotation WRITE setAxisLabelsRotation)
    Q_PROPERTY(QString axisLabelsFormat READ axisLabelsFormat WRITE setAxisLabelsFormat)
    Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor)
    Q_PROPERTY(QColor axisColor READ axisColor WRITE setAxisColor)
    Q_PROPERTY(QColor axisTitleColor READ axisTitleColor WRITE setAxisTitleColor)
    Q_PROPERTY(QString axisTitle READ axisTitle WRITE setAxisTitle)
    Q_PROPERTY(QFont axisTitleFont READ axisTitleFont WRITE setAxisTitleFont)

    Q_PROPERTY(double upperBound READ upperBound WRITE setUpperBound)
    Q_PROPERTY(double lowerBound READ lowerBound WRITE setLowerBound)
    Q_PROPERTY(QDateTime lowerBoundDateTime READ lowerBoundDateTime WRITE setLowerBoundDateTime)
    Q_PROPERTY(QDateTime upperBoundDateTime READ upperBoundDateTime WRITE setUpperBoundDateTime)
    Q_PROPERTY(bool axisAutoscaleEnabled READ axisAutoscaleEnabled WRITE setAxisAutoscaleEnabled)
    Q_PROPERTY(double axisLabelDist READ axisLabelDist WRITE setAxisLabelDist)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(qreal zValue READ zValue WRITE setZValue)

public:

    enum { Type = UserType + 2 };

    /* Enable the use of qgraphicsitem_cast with this item. */
    virtual int type() const
    {
        return Type;
    }

    enum Orientation { Horizontal, Vertical };

    /** \brief the id of the ScaleItem (kind of axis)
      *
      * PlotSceneWidget manages the two default axes for you, and gives them
      * the xBottom and yLeft ids, respectively.
      * If you need more axes, add them with the other available id or use
      * another value of your choice.
      */
    enum Id  { xBottom, yLeft, xTop, yRight, User = 100 };

    virtual ~ScaleItem();

    ScaleItem(Orientation o, QGraphicsPlotItem *parent, Id id);

    void setOrientation(Orientation o);

    Orientation orientation() const;

    Id axisId() const;

    double tickStepLen() const;

    double axisLabelsRotation() const;

    QString axisLabelsFormat() const;

    bool gridEnabled() const;

    bool axisLabelsEnabled() const;

    QColor gridColor() const;

    QColor axisColor() const;

    QRectF boundingRect () const;

    double upperBound() const;

    double lowerBound() const;

    QDateTime lowerBoundDateTime() const;

    QDateTime upperBoundDateTime() const;

    QDateTime doubleToDateTime(double d) const;

    bool axisAutoscaleEnabled() const;

    void adjustScaleBounds(double newMin, double newMax);

    double axisLabelDist() const;

    QFont font() const;

    QFont axisTitleFont() const;

    QColor axisTitleColor() const;

    QString axisTitle() const;

    /* implementation of ItemChangeListener
     *
     */
    virtual void fullVectorUpdate();

    void affectingBoundsPointsRemoved();

    /** \brief implementation of PlotChangeListener::plotRectChanged interface
      *
      */
    virtual void plotRectChanged(const QRectF &newRect);

    virtual void plotAreaChanged(const QSizeF &area);

    virtual void scrollBarChanged(Qt::Orientation orientation, int value);

    virtual void plotZoomLevelChanged(int level);

    /** \brief implementation of PlotChangeListener::sceneContentChanged interface
      *
      */
    virtual void itemsAboutToBeDrawn();

    void installAxisChangeListener(AxisChangeListener *l);

    void removeAxisChangeListener(AxisChangeListener *l);

    void installScaleLabelInterface(ScaleLabelInterface *iface);

    ScaleLabelInterface *scaleLabelInterface() const;

    void removeScaleLabelInterface();

    QString label(double value) const;

    double maxLabelWidth() const;

    double plotAreaW, scaledCanvasWidth;

    double plotAreaH, scaledCanvasHeight;

    QRectF plotArea() const;

    void setAutoscaleMargin(double spanPercent);

    double autoscaleMargin() const;

public slots:

    void setTickStepLen(double len);

    void setAxisLabelsEnabled(bool en);

    void setAxisLabelsRotation(double angle);

    void setAxisLabelsFormat(const QString& fmt);

    void setGridEnabled(bool en);

    void setGridColor(const QColor& c);

    void setAxisColor(const QColor& c);

    void setUpperBound(double ub);

    void setLowerBound(double lb);

    void setLowerBoundDateTime(const QDateTime& t);

    void setUpperBoundDateTime(const QDateTime& t);

    void setAxisAutoscaleEnabled(bool en);

    void setBounds(double lowerBound, double upperBound);

    bool setBoundsFromCurves();

    void setAxisLabelDist(double d);

    void setFont(const QFont& f);

    void setAxisTitleFont(const QFont &fo);

    void setAxisTitleColor(const QColor & co);

    void setAxisTitle(const QString & ti);

    void updateLabelsCache();

signals:

    /** \brief this signal is emitted when the axis upper bound changes
      *
      * @param max the new axis upper bound
      *
      * @see lowerBoundChanged
      */
    void upperBoundChanged(double max);

    /** \brief this signal is emitted when the axis lower bound changes
      *
      * @param max the new axis lower bound
      *
      * @see upperBoundChanged
      */
    void lowerBoundChanged(double min);

    /** \brief this signal notifies that the auto scale property has changed
      *
      * @param enabled true the auto scale mode is enabled
      * @param enabled false the auto scale mode is disabled
      *
      * @see setAxisAutoscaleEnabled
      * @see axisAutoscaleEnabled
      */
    void autoscaleEnabledChanged(bool enabled);


protected:

    void mNotifyBoundsChanged();

    void updateLabelsFormat(const QString& desiredFormat);

    double updateStepLen();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

private:
    ScaleItemPrivate* d;

    int mGetDecimals(double q);

    void mRecalculateAxisTitleSize();
};

#endif // SCALEITEM_H
