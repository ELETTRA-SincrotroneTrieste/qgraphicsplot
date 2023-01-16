#ifndef QGRAPHICSPLOTITEM_H
#define QGRAPHICSPLOTITEM_H

#include <QGraphicsObject>
#include <QPair>
#include "scaleitem.h"
#include "xyplotinterface.h"

#if QT_VERSION < 0x040800
#error "Please use at least Qt 4.8 to build this library and the applications using it"
#endif

class QGraphicsPlotItemPrivate;
class PointData;
class Point;
class CurveChangeListener;
class SceneCurve;
class AxesManager;
class PlotGeometryEventListener;
class MouseEventListener;
class LegendItem;
class QGraphicsZoomer;

/** \brief The main class that contains the plot canvas.
  *
  * \mainpage
  *
  * This library can draw curves on a plot canvas.
  * Each curve is represented by a SceneCurve. A SceneCurve contains the data of the curve.
  * The SceneCurve is not responsible for drawing its shape.
  * Each SceneCurve must be given a name and added to the plot by means of QGraphicsPlotItem::addCurve
  * <br/>
  * The painting takes place by a CurveItem object constructed by passing a SceneCurve to it
  * and by an implementation of an ItemPainterInterface which must be installed on a CurveItem.
  *
  *
  * The ScaleItem class represents an axis scale. More than a couple of axes may be added to the
  * plot. AxesManager class is responsible of coupling and managing the axes.
  *
  * The TargetItem item is a QGraphicsObject that can be moved over the plot and it is aimed at
  * realizing writers.
  *
  * Readers that want to draw curves on the plot have to conform to the XYPlotInterface
  * interface that the QGraphicsPlotItem implements.
  *
  * In the <em>examples</em> directory you will find some examples of the usage of the library.
  *
  * @see QGraphicsPlotItem
  * @see SceneCurve
  * @see CurveItem
  * @see ItemPainterInterface
  * @see LinePainter
  * @see DotsPainter
  * @see ScaleItem
  *
  * \par Examples

  * \par Scalar plot
  * see directory
  * examples/scalar
  * \image html scalar.png
  *
  * \par Spectrum plot
  * see directory examples/spectrum/
  * \image html spectrum.png
  *
  * \par Aging circles
  * see directory
  * examples/agingcircles
  *
  * \image html agingCircles.png
  *
  * \par Using external scales
  * External scales can be used and they can be useful when the plot is zoomed and the plot
  * internal axes fall out of the zoom area.
  * You can find an example in the directory
  * examples/externalscales
  * @see ExternalScaleWidget
  *
  * \image html externalScales.png
  *
  * \par Properties and settings
  * Each QObject (in particular QGraphicsObject for the items in a plot) can export its relevant
  * properties through a set of <em>Q_PROPERTY</em> declarations.
  * These properties are then taken into account at runtime and the most important  types of
  * properties can be modified and saved.
  * Each object will display its properties under a page of a tab widget.
  * To register QObject's properties, the object must have an objectName and you must provide
  * a settings key through the setSettingsKey method call, passing a string as a key for the properties
  * of your objects to be saved by the QSettings class. Normally, you want to pass the application name
  * as settings key.
  *
  * \note Since version 2.2.2, Qt <em>dynamic properties</em> directly set on QObjects with
  * <em>setProperty</em> are taken into account and made available by the configuration dialog.
  * \note Dynamic properties starting with two underscores <em>"__"</em> are reserved and do not
  * have to be used.
  * \note Since dynamic properties are expected to be application specific, their names are put in
  * evidence by means of a bold font in the configuration dialog.
  *
  * \code
  * graphicsPlot->setSettingsKey(qApp->applicationName());
  * \endcode
  *
  * Each configurable object must then be registered with the addConfigurableObjects method call,
  * passing the name of the tab in the tab widget for the object and a reference to the object
  * itself.
  *
  * \code
  * graphicsPlot->addConfigurableObjects(curveItem->name() + " Properties", curveItem);
  * \endcode
  *
  * \par Position of the axes
  * The axes are placed by QGraphicsPlotItem through the classes AxesManager and AxisCouple.
  * Each AxisCouple in the QGraphicsPlotItem represents a couple of axis, one with Horizontal
  * orientation and the other with Vertical orientation.
  * The default positioning of the axes is done by default such as each the axes cross each
  * other in the middle.
  * If you want to change the reciprocal axes position, see QGraphicsPlotItem::setOriginPosPercentage
  * QGraphicsPlotItem::setDefaultXAxisOriginPosPercentage and
  * QGraphicsPlotItem::setDefaultYAxisOriginPosPercentage.
  *
  * \code
  *  // change the position of the axis
  *  graphicsPlot->setDefaultXAxisOriginPosPercentage(0.0);
  *  graphicsPlot->setDefaultYAxisOriginPosPercentage(0.1);
  * \endcode
  *
  * The code above produces the effect of placing the scales as in the <em>scalartime</em>
  * example in the <em>examples</em> folder.
  * Here you can see a screenshot of the <em>scalartime</em> example
  *
  * \image html scalartime.png
  *
  * In the <em>scalartime</em> example, the time labels in the x axis have been realized by
  * installing a TimeScaleLabel on the x axis:
  *
  * \code
  *  TimeScaleLabel *timeScaleLabel = new TimeScaleLabel();
  *  graphicsPlot->xScaleItem()->installScaleLabelInterface(timeScaleLabel);
  * \endcode
  *
  * @see TimeScaleLabel
  * @see installScaleLabelInterface
  * @see ScaleLabelInterface
  *
  * To customize x or y scale labels, have a look to ScaleLabelInterface. You must
  * subclass that interface and provide a ScaleLabelInterface::label method
  * implementation.
  *
  * \par Implementation notes
  * PlotGeometryEventListener and MouseEventListener interfaces can benefit from
  * QGraphicsPlotItem notifications when the geometry of the plot changes (see
  * PlotGeometryEventListener) or the mouse is clicked/moved/pressed/released on a certain point
  * (see MouseEventListener).
  * installMouseEventListener and installPlotGeometryChangeListener methods can be used
  * by any class implementing the MouseEventListener and PlotGeometryEventListener
  * interfaces in order to register for the corresponding events.
  * <br/>
  * Other relevant changes taking place in the QGraphicsPlotItem are notified by means of
  * Qt <em>signals</em>. For example, signals are emitted when a new curve is added or
  * removed or an axis is removed. Appropriate signals are also emitted when the area
  * of the plot changes (taking into account the currently applied transform matrix)
  * and when the plot rect changes.
  *
  */
class  QGraphicsPlotItem : public QGraphicsObject, public XYPlotInterface
{
    Q_OBJECT
    Q_PROPERTY(bool xScaleEnabled READ xScaleEnabled WRITE setXScaleEnabled)
    Q_PROPERTY(bool yScaleEnabled READ yScaleEnabled WRITE setYScaleEnabled)
    Q_PROPERTY(bool mouseZoomEnabled READ mouseZoomEnabled WRITE setMouseZoomEnabled)
    Q_PROPERTY(bool scaleSceneOnResizeEnabled READ scaleSceneOnResizeEnabled WRITE setScaleSceneOnResizeEnabled)
    Q_PROPERTY(int refreshPeriod READ refreshPeriod WRITE setRefreshPeriod)
    Q_PROPERTY(double defaultXAxisOriginPosPercentage READ defaultXAxisOriginPosPercentage WRITE setDefaultXAxisOriginPosPercentage)
    Q_PROPERTY(double defaultYAxisOriginPosPercentage READ defaultYAxisOriginPosPercentage WRITE setDefaultYAxisOriginPosPercentage)
    Q_PROPERTY(bool legendVisible READ legendVisible WRITE setLegendVisible)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:

    enum Axis { xBottom, yLeft, xTop, yRight, User = 100 };

    explicit QGraphicsPlotItem(QGraphicsItem *parent = nullptr);
    explicit QGraphicsPlotItem(bool initDefaultAxes,  QGraphicsItem *parent = nullptr);
    virtual ~QGraphicsPlotItem();

    bool xScaleEnabled() const;
    bool yScaleEnabled() const;
    bool scaleSceneOnMouseScroll() const;
    bool sceneRectToWidgetGeometry() const;
    bool mouseZoomEnabled() const;
    bool modifiedPaintEvent() const;
    bool smoothPixmapTransform() const;
    bool legendVisible() const;

    virtual void boundsChanged();

    void installPlotGeometryChangeListener(PlotGeometryEventListener *l);
    void removePlotGeometryChangeListener(PlotGeometryEventListener *l);

    void installMouseEventListener(MouseEventListener *l);
    void removeMouseEventListener(MouseEventListener* l);

    QColor backgroundColor() const;

    QList<Point *> getPoints() const;

    /** \brief shortcut to retrieve the x scale item (ScaleItem::xBottom) which is always
      *        created by default.
      *
      * @return the x bottom scale item, if not removed programmatically, NULL otherwise.
      */
    ScaleItem *xScaleItem() const;

    /** \brief shortcut to retrieve the y scale item (ScaleItem::yLeft) which is always
      *        created by default.
      *
      * @return the y left scale item, if not removed programmatically, NULL otherwise.
      */
    ScaleItem *yScaleItem() const;

    /** \brief returns the ScaleItem associated to the argument.
      *
      *
      * @return the ScaleItem associated to the id. The ScaleItem must have previously been
      *         programmatically added through the AxesManager.
      *
      * Returns NULL if the ScaleItem with the provided Id was not added programmatically.
      *
      * @see AxesManager::addCouple
      */
    ScaleItem* scaleItem(ScaleItem::Id id) const;

    ScaleItem* associatedAxis(ScaleItem::Id otherAxisId) const;

    QPair<double, double> associatedOriginPosPercentage(ScaleItem::Id xAxisId, ScaleItem::Id yAxisId, bool *ok) const;

    void addConfigurableObjects(const QString& title, QObject *object);
    void removeConfigurableObjects(const QString& title);

//    /** \brief a key to be used to retrieve the settings for the plot.
//      *
//      * For instance, it may be the associated device name or attribute or anything else
//      * needed to distinguish this particular plot from the other plots which may be present
//      * in the same application or another one.
//      */
//    void setSettingsKey(const QString& key);
//    QString settingsKey() const;
    void loadConfigurationProperties();

    QPointF invTransform(const QPointF& pSceneCoord);
    double invTransform(const double xCoord, ScaleItem* scaleItem) const;

    /** \brief transforms a value on the scaleItem axis in axis coordinates into a
      *        value in scene coordinates inside the plot rect.
      *
      * @param x the value of the point on the ScaleItem axis
      * @param scaleItem the axis on which to make the calculation.
      * @return a coordinate in scene coordinates, representing the x axis point mapped
      *         into a point in the scene coordinate
      */
    double transform(const double x, ScaleItem* scaleItem) const;
    QPointF invTransform(const QPointF& pSceneCoord, ScaleItem* xScaleItem, ScaleItem* yScaleItem) const;

    void addCurve(SceneCurve *curve);

    /** \brief Creates a new curve with the given name and automatically associates it to the
      *        x bottom axis and y left axis.
      *
      * @return the new curve just created. You can configure its properties
      *         using this return value.
      *
      * @param name the name with which the curve is marked and identified.
      */
    SceneCurve * addCurve(const QString& name);

    /** \brief Creates a new curve with the given name and attachs the curve to the specified
      *        x and y ScaleItems (axes).
      *
      * Use this method instead of the one provided above to attach the curve to a couple of
      * axes different from the x bottom and y left default axes.
      *
      */
    SceneCurve *addCurve(const QString &name, ScaleItem *xScaleItem, ScaleItem *yScaleItem);

    /** \brief finds the curve with the provided name
      *
      * @return the curve with the given name or NULL if not present
      *
      * @param name the name to look for
      */
    SceneCurve *findCurve(const QString& name);

    /** \brief returns a list of all curves belonging to this plot
      *
      */
    QList<SceneCurve *> getCurves() const;

    SceneCurve *addLineCurve(const QString& name, ScaleItem *xScaleItem = NULL, ScaleItem *yScaleItem = NULL);

    void curveAboutToBeDestroyed(const QString& name, bool deleteCurve = true);

    /** \brief returns the list of curves that are attached to the provided axisId with Orientation
      *        orientation
      *
      */
    QList<SceneCurve *> curvesForAxes(ScaleItem::Id axisId, ScaleItem::Orientation orientation) const;

    /** \brief returns the reference to the AxesManager.
      *
      * @see AxesManager
      *
      * The AxesManager class manages the axes in the plot and their origins.
      */
    AxesManager *axesManager() const;

    QRectF plotRect() const;

    double originPosPercentage(ScaleItem *scaleIt) const;
    double defaultXAxisOriginPosPercentage() const;
    double defaultYAxisOriginPosPercentage() const;

    double xAxisLowerBound() const;
    double yAxisLowerBound() const;
    double xAxisUpperBound() const;
    double yAxisUpperBound() const;

    bool xAxisAutoscaleEnabled() const;
    bool yAxisAutoscaleEnabled() const;
    bool xAxisLabelsOutsideCanvas() const;
    bool yAxisLabelsOutsideCanvas() const;

    /** \brief returns the refresh period of the internal timer used to refresh
      *        the scene when manualSceneUpdate is turned on.
      *
      * @return the period in milliseconds of the internal timer. -1 is returned
      *         if the timer is not active, i.e. manualSceneUpdate is false
      *
      * @see setManualSceneUpdate
      * @see manualSceneUpdate
      * @see setRefreshPeriod
      */
    int refreshPeriod() const;

    /** \brief retrieve the list of curves that are closest to the viewPos point in view coordinates.
               The closestPos point is the point, in scene coordinates, closest to the viewPos and
               belonging to the closest curves. The closestIndex is the x axis index coordinate
               of the closest point.
      *
      * The returned list contains more than one curve if they overlap in the same point.
      * Otherwise, the list will contain at most one curve.
      *
      * @param closestPos the position of the closest point, in scene coordinates.
      * @param viewPos the position, in view coordinates, whose curve's closest point must be searched
      * @param closestIndex the index with which you can retrieve the data in the data vector of
      *        the closest curve(s).
      *
      * An example can be found in the MarkerItem::mouseClickEvent method
      * <br/>
      * If no closest curve is found, the returned list is empty, the closestPos is a null point
      * and the closest index is set to -1.
      *
      * closestPos is passed by reference
      * closestIndex is a pointer to an integer that will point to the index of the data vector
      *              associated to the closest point in the closest curve
      * viewPos is the position in view coordinates, it may represent, as in MarkerItem::mouseClickEvent,
      *         the point where a click event has taken place in the QGraphicsPlotItem.
      *
      *
      *
      * @see MarkerItem::mouseClickEvent
      */
    QList<SceneCurve *> getClosest(QPointF &closestPos, int *closestIndex, const QPointF &viewPos);

    bool inZoom() const;
    int zoomLevel() const;
    QGraphicsZoomer * zoomer() const;

    LegendItem *legendItem() const;

    ScaleItem *addAxis(ScaleItem::Orientation o, ScaleItem::Id id, ScaleItem *associatedAxis);
    void removeAxis(ScaleItem::Id id);
    void clearAxes(bool all = false, ScaleItem::Orientation o = ScaleItem::Vertical);

    void clear();

signals:

    /** \brief the plot was clicked at the position pos
     *
     * @param pos the point where the click took place.
     */
    void clicked(const QPointF& pos);

    /** \brief The plot rectangle has changed.
     *
     * @param newRect the new rectangle containing the plot.
     *
     * \par Plot Rect property.
     * It takes into account the sceneRect adjusted with the
     * plotAreaPercentageWidth, plotAreaTopLeftXPercentage,
     * plotAreaPercentageHeight and plotAreaTopLeftYPercentage
     * properties. For this, newRect does not always coincide with
     * the scene rect.
     *
     * \note
     * The rectangle does not take into account the <em>transform</em>
     * being applied, if any. For that purpose, please see plotAreaChanged
     *
     * @see plotAreaChanged
     */
    void plotRectChanged(const QRectF& newRect);

    /** \brief notifies that the plot area has changed
     *
     * @param newSize the new size of the plot area
     *
     * \par Note
     * newSize takes into account the transform() being applied to the view.
     * The area is calculated by scaling the plotRect property according to
     * the m11 and m22 elements of the transform matrix.
     */
    void plotAreaChanged(const QSizeF& newSize);

    /** \brief this signal is emitted when a curve is removed from the plot
     *
     * Any class interested in knowing when a curve is removed (e.g. MarkerItem)
     * must be connected to this signal, which is emitted right before the
     * SceneCurve passed as parameter is removed.
     *
     * @param curve a reference to the curve being removed
     *
     * \par Note
     * When this signal is emitted, the curve axes have not been already detached
     * (they are available) and also the associated CurveItem is still available.
     *
     * @see CurveItem
     * @see ScaleItem
     * @see CurveAddded
     *
     */
    void curveAboutToBeRemoved(SceneCurve *c);

    /** \brief This signal notifies that the curve has been added to the plot
     *
     * @param curve the new curve
     *
     * \note
     * The AxisChangeListener on the x and y axes has been installed
     * on the plot when this signal is emitted. The curve has already been
     * inserted in the internal list of curves. This means it is available
     * through getCurves and findCurve.
     * This does not mean that a CurveItem has already been installed on the curve.
     * Always check for curveItem() returning NULL.
     * If you use addLineCurve, then the curve has a CurveItem already associated
     * when the signal is emitted.
     */
    void curveAdded(SceneCurve *curve);

    /** \brief Notifies that the scale factors have changed.
     *
     * Signal notifying that the scale factors of the view have changed
     *
     * @param dx the m11 transform matrix value
     * @param dy the m22 transform matrix value
     *
     */
    void viewScaleChanged(double dx, double dy);

    /** \brief Notifies that the scale factors have changed. The operations
     *         performed by the QGraphicsPlotItem and its zoomer scale the view
     *         by the same factor in order to keep the aspect ratio.
     *
     * Notifies that the scale factors have changed. The operations
     * performed by the QGraphicsPlotItem and its zoomer scale the view
     * by the same factor in order to keep the aspect ratio.
     * So this method is provided, with a single scale parameter.
     *
     * \note If m11 differs from m22, then their mean is calculated and the mean
     * value is emitted.
     *
     */
    void viewScaleChanged(double scale);
    
public slots:

    void setXScaleEnabled(bool en);
    void setYScaleEnabled(bool en);

    void setScaleSceneOnResizeEnabled(bool en);

    void setMouseZoomEnabled(bool en);

    bool scaleSceneOnResizeEnabled() const;



    /** \brief enable or disable the scene scaling feature when the wheel mouse is scrolled
      *
      * @param en true enable scene scaling
      * @param en false disable scene scaling
      */
    void setScaleSceneOnMouseScroll(bool en);

    /** \brief This is an important optimization that acts on the QGraphicsView::ViewportUpdateMode
      *        that comes into relevance when you have several items that must be fastly drawn.
      *
      * This property sets the ViewportUpdateMode to <strong>QGraphicsView::NoViewportUpdate</strong>
      * meaning that QGraphicsView will never update its viewport when the scene changes;
      * the user is expected to control all updates. This mode disables all (potentially slow)
      * item visibility testing in QGraphicsView, and is suitable for scenes that either
      * require a fixed frame rate, or where the viewport is otherwise updated externally.
      *
      * <h3>Note</h3>
      * <p>
      * When you enable manual scene update, then you must call setRefreshPeriod with the
      * desired refresh period of the scene, which typically must be equal to the fastest
      * refresh rate needed by the fastest item.
      * <br/>
      * Calling setManualSceneUpdate with a false parameter, restores QGraphicsScene default
      * ViewportUpdateMode, which is QGraphicsView::MinimalViewportUpdate, according to the
      * Qt documentation.
      *
      *
      * </p>
      *
      * @param manual true update is managed by an internal timer. setRefreshPeriod must be called
      *        with a greater than zero value in order to activate the timer and have the scene
      *        refreshed with that interval.
      *
      * @param manual false the update is set to QGraphicsView::MinimalViewportUpdate, the default
      *        Qt setting for the ViewportUpdateMode property. The internal refresh timer, if previously
      *        activated and started with a valid setRefreshPeriod method call is <strong>automatically</strong>
      *        stopped for you. The timer is not deleted, it is just stopped, so that a subsequent call
      *        to setManualSceneUpdate with a true parameter <strong>automatically restarts the timer
      *        with the timeout previously set with setRefreshPeriod</strong>.
      *        In this way, a second manual mode activation does not need another call to setRefreshPeriod.
      *        A call to setRefreshPeriod with zero or a negative value instead produces the <strong>
      *        destruction</strong> of the timer.
      *
      *
      * @see setRefreshPeriod
      * @see refreshPeriod
      * @see manualSceneUpdate
      */
    void setManualUpdate(bool manual);

    /** \brief sets the refresh period of the internal timer used to refresh
      *        the scene when manualSceneUpdate is turned on.
      *
      * @param  period  the interval, in milliseconds of the internal timer.
      *
      * The period must be greater than 0. If zero or a negative value is passed,
      * then the internal timer is stopped and destroyed.
      * When invoked with a value greater than zero, an internal refresh timer is
      * created, with objectName "refreshTimer", its interval is set to period and
      * the timer is started. At each timeout, the scene is refreshed.
      * The period must be as fast as to ensure that the fastest item in the scene is
      * updated in time.
      *
      *
      * @see setManualSceneUpdate
      * @see manualSceneUpdate
      * @see refreshPeriod
      */
    void setRefreshPeriod(int period);

    void executePropertyDialog();

    virtual void appendData(const QString& curveName, double x, double y);

    virtual void appendData(const QString& curveName, const QVector<double>& xData, const QVector<double> &yData);

    virtual void setData(const QString& curveName,
                         const QVector< double > &xData,
                         const QVector< double > &yData);

    virtual void setData(const QString& curveName,
                         const QVector< double > &yData);



    void setOriginPosPercentage(ScaleItem *scaleIt, double percent);
    void setDefaultXAxisOriginPosPercentage(double percent);
    void setDefaultYAxisOriginPosPercentage(double percent);

    void setLegendVisible(bool visible);

    void setXAxisLowerBound(double xlb);

    void setYAxisLowerBound(double ylb);

    void setXAxisUpperBound(double xub);

    void setYAxisUpperBound(double yub);

    void setXAxisAutoscaleEnabled(bool en);

    void setYAxisAutoscaleEnabled(bool en);

    void setXAxisLabelsOutsideCanvas(bool outside);

    void setYAxisLabelsOutsideCanvas(bool outside);

    void setBackgroundColor(const QColor&) const;

    /* end scene area configuration methods */

    void saveData();

    virtual QMenu *createContextMenu();

    QRectF boundingRect() const;

protected slots:
    void curveAboutToBeDestroyed(QObject *crv);

    void removeAxis(QObject *axis);

protected:

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

//    void wheelEvent(QGraphicsSceneMouseEvent *event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

    void mSwitchAxesCurvesForeground();

    /** \brief Recalculates the plot rect and caches it.
      *
      * The cached plot rect must be recalculated when
      * <ul><li>the scene rect changes</li>
      * <li>the top left percent (x or y) values change</li>
      * <li>the width or height percentage of the plot area change</li>
      * </ul>
      */
    void recalculatePlotRect();

//    void resizeEvent(QResizeEvent *event);

    void notifyPlotAreaChanged();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    void update(const QRectF &area= QRectF());
protected slots:

    void sceneRectChanged(const QRectF &);

    void hScrollBarValueChanged(int value);

    void vScrollBarValueChanged(int value);

private:
    QGraphicsPlotItemPrivate *d;

    void initPlot();

    void initDefaultAxes();

};

#endif // XYSCENE_H
