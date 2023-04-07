#include "qgraphicsplotitem.h"
#include "curve/point.h"
#include "curve/pointdata.h"
#include "qgraphicsplotmacros.h"
#include "qgraphicsplotitem_private.h"
#include "curvechangelistener.h"
#include "properties/settingsloader.h"
#include "colors.h"
#include "curve/scenecurve.h"
#include "curve/curveitem.h"
#include "painters/linepainter.h"
#include "axiscouple.h"
#include "axesmanager.h"
#include "axiscouple.h"
#include "mouseeventlistener.h"
#include "qgraphicszoomer.h"
#include "colorpalette.h"
#include "scalelabelinterface.h"
#include "items/legenditem.h"
#include "plotsaver/plotscenewidgetsaver.h"
#include <QGLWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QtDebug>
#include <QMetaType>
#include <QCoreApplication>
#include <QVector>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QMenu>
#include <QTimer>
#include <QScrollBar>
#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

#include "properties/propertydialog.h"


/** \brief PlotSceneWidget is a QGraphicsView with a QGraphicsScene where QGraphicsItem items can
 *         be placed and displayed. In particular, PlotSceneWidget is a <em>plot</em> where
 *         <em>curves</em> can be drawn.
 *
 * \par PlotSceneWidget
 * PlotSceneWidget is a QGraphicsView with a QGraphicsScene where QGraphicsItem items can
 *         be placed and displayed. In particular, PlotSceneWidget is a <em>plot</em> where
 *         <em>curves</em> can be drawn.
 *
 * \par Axes
 * A PlotSceneWidget has by default a couple of axes, the x and y axes, and their origin is by
 * default in the center of the scene.
 * If you want to change the axes position, see
 * \li PlotSceneWidget::setOriginPosPercentage
 * \li PlotSceneWidget::setDefaultXAxisOriginPosPercentage and
 * \li PlotSceneWidget::setDefaultYAxisOriginPosPercentage.
 *
 * \par Reducing the plot area inside the scene.
 * It is possible to reduce the area occupied by the plot by means of the plotAreaPercentageWidth,
 * plotAreaPercentageHeight, plotAreaTopLeftXPercentage, plotAreaTopLeftYPercentage properties.
 *
 *
 * \par Curves
 * Each curve is represented by a SceneCurve. A SceneCurve contains the data of the curve.
 * The SceneCurve is not responsible for drawing itself.
 * Each SceneCurve must be given a name and added to the plot by means of PlotSceneWidget::addCurve
 * <br/>
 * The painting takes place by a CurveItem object instantiated by passing a SceneCurve to its constructor
 * and by an implementation of an ItemPainterInterface which must be installed on a CurveItem.
 * Once a CurveItem is constructed with a SceneCurve, the SceneCurve <em>takes the ownership
 * of the CurveItem, destroying it when the curve is destroyed</em>.
 * Once an ItemPainterInterface implementation instance is installed on a CurveItem, the
 * CurveItem <em>takes the ownership of the item painter</em> destroying it on CurveItem
 * destruction.
 *
 * LinePainter is an example of an ItemPainterInterface implementation.
 * DotsPainter is another example.
 *
 * \par Example. Adding a curve to a PlotSceneWidget
 * \code
 * SceneCurve *curve = myPlotSceneWidget->addCurve("Curve Name");
 * //
 * // if you want no more than bufsize points on the curve
 * int bufsize = 1000;
 * curve->setBufferSize(bufsize);
 *
 * // Create the effective curve painter!
 * // Just pass the curve in the constructor
 * // The SceneCurve curve takes the ownership of curveItem, i.e. CurveItem
 * // will be destroyed when removeCurve is called on PlotSceneWidget (or,
 * // equivalently, on SceneCurve destruction)
 * //
 * CurveItem *curveItem = new CurveItem(curve);
 * // add the CurveItem to the Graphics Scene
 * scene()->addItem(curveItem);
 * //
 * // The CurveItem is aimed at calculating the transformations between the curve data
 * // coordinates and the PlotSceneWidget pixel coordinates.
 * // The CurveItem delegates the painting to specialized painters, which can be
 * // line painers, dot painters, histogram painters, and so on.
 * //
 * // Let's create a LinePainter to connect points on the plot with lines:
 * // Pass the curve item as parameter in the LinePainter constructor
 * // The curveItem takes the ownership of the line painter.
 * //
 * LinePainter *linePainter = new LinePainter(curveItem);
 * linePainter->setLineColor(Qt::green); //  colour the lines
 *
 * // setData on the curve
 * QVector<double> xData, yData;
 * // fill the vectors xData and yData as you like, then....
 * // ...
 * //
 * curve->setData(xData, yData);
 *
 * \endcode
 *
 * \par Why ownership of CurveItem and LinePainter is passed to SceneCurve and CurveItem respectively?
 * This particular implementation allows you to create the CurveItem and one or more LinePainter
 * objects and forget about their memory management.
 * It is strongly suggested to use addCurve and removeCurve(const QString&) methods to add and
 * remove curves to and from the PlotSceneWidget
 * @see addCurve
 * @see removeCurve(const QString&)
 * This aspect <em>must be taken into account when your application allows curve deletion
 * at runtime</em>. In other cases, you never want to worry about deleting the objects you
 * create and attach to your plot to add curves or decorate them. This is accomplished by
 * Qt QObject children deletion and careful design of parent-children hierarchy in QGraphicsPlot
 * library.
 *
 * \par Some more details about item deletion.
 * \li<em>CurveItem</em> is a QGraphicsObject and when it's added to a QGraphicsScene through QGraphicsScene::addItem
 * the ownership is taken by the QGraphicsScene. This means that every CurveItem is deleted on scene
 * deletion, i.e. on PlotSceneWidget deletion, since the plot scene is a child of PlotSceneWidget.
 * \li<em>SceneCurve</em> is created by PlotSceneWidget::addCurve as a child of PlotSceneWidget. Each
 * curve  is thus deleted on PlotSceneWidget destruction.
 * \li <em>LinePainter</em>, as in the <em>example above</em>, must be constructed by passing a CurveItem
 * in its constructor. <em>All ItemPainterInterface implementations</em>, such as LinePainter, must
 * inherit from QObject and the parent must be a CurveItem, as you can see in the LinePainter
 * implementation. In this way, on CurveItem deletion, the LinePainter is automatically deleted, as a
 * child of CurveItem.
 *
 * \par Curve removal.
 * You can remove a SceneCurve at runtime by calling PlotSceneWidget::removeCurve.
 * This method will delete the associated CurveItem (and all its painters, such as LinePainter,
 * for example) and then delete the SceneCurve itself.
 * @see removeCurve
 * @see addCurve
 *
 * \par Using external scales
 * External scales can be used and they can be useful when the plot is zoomed and the plot
 * internal axes fall out of the zoom area.
 * @see ExternalScaleWidget
 *
 *
*/
QGraphicsPlotItem::QGraphicsPlotItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    d = new QGraphicsPlotItemPrivate();
    initPlot();
    initDefaultAxes();
}


QGraphicsPlotItem::QGraphicsPlotItem(bool initDefAxes, QGraphicsItem *parent):
    QGraphicsObject(parent)
{
    d = new QGraphicsPlotItemPrivate();
    initPlot();
    if(initDefAxes)
        initDefaultAxes();
}

QGraphicsPlotItem::~QGraphicsPlotItem() {
    delete d;
}

void QGraphicsPlotItem::initPlot()
{
    qRegisterMetaType<PointData>("PointData");
    qRegisterMetaType<QVector<PointData> >("QVector<PointData>&");
    d->plotRect = QRectF(0, 0, 400, 300);
    d->axesManager = NULL;

    /* zoom manager */
    d->zoomer = new QGraphicsZoomer(this);

    /* for each key of the map, create a tab page with all the widgets properties */
    addConfigurableObjects("Plot", this);

    //    connect(scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));

    /* legend */
    d->legendItem = new LegendItem(this);
    /* set it outside the scene rect until it becomes visible */
    d->legendItem->setPos(5, 5);
    connect(this, SIGNAL(curveAdded(SceneCurve*)), d->legendItem, SLOT(newCurveAdded(SceneCurve*)));
    connect(this, SIGNAL(curveAboutToBeRemoved(SceneCurve*)), d->legendItem, SLOT(curveRemoved(SceneCurve*)));
    d->legendItem->setVisible(false);
    d->legendItem->setObjectName("PlotSceneWidgetLegendItem");
    //    scene->addItem(d_ptr->legendItem);
    addConfigurableObjects("Legend", d->legendItem);
}

void QGraphicsPlotItem::initDefaultAxes()
{
    /* x and y scales enabled by default: create them */
    d->axesManager = new AxesManager();
    ScaleItem *xScaleItem = addAxis(ScaleItem::Horizontal, ScaleItem::xBottom, NULL);
    ScaleItem *yScaleItem = addAxis(ScaleItem::Vertical,  ScaleItem::yLeft, xScaleItem);

    addConfigurableObjects("X Axis", xScaleItem);
    addConfigurableObjects("Y Axis", yScaleItem);

    setXScaleEnabled(true); /* shows and positions them */
    setYScaleEnabled(true);
}

ScaleItem* QGraphicsPlotItem::addAxis(ScaleItem::Orientation o, ScaleItem::Id id, ScaleItem* associatedAxis)
{
    if(!d->axesManager)
        d->axesManager = new AxesManager();

    AxisCouple *co = d->axesManager->findIncompleteCouple(associatedAxis);
    ScaleItem *scaleItem = new ScaleItem(o, this, id);
    scaleItem->setPos(0,0);
    scaleItem->setObjectName(QString("%1: scaleItem %2").arg(o == ScaleItem::Horizontal ? "x" : "y").arg(id));
    installPlotGeometryChangeListener(scaleItem);
    if(co && o == ScaleItem::Horizontal && co->yAxis != NULL)
        co->xAxis = scaleItem;
    else if(co && o == ScaleItem::Vertical && co->xAxis != NULL)
        co->yAxis = scaleItem;
    else if(!co && o == ScaleItem::Horizontal
            && (associatedAxis == NULL || associatedAxis->orientation() == ScaleItem::Vertical)) /* add a new axis couple */
    {
        d->axesManager->addCouple(AxisCouple(scaleItem, associatedAxis, 0.5, 0.5));
    }
    else if(!co && o == ScaleItem::Vertical
            && (associatedAxis == NULL || associatedAxis->orientation() == ScaleItem::Horizontal)) /* add a new axis couple */
    {
        d->axesManager->addCouple(AxisCouple(associatedAxis, scaleItem, 0.5, 0.5));
    }
    else
        perr("PlotSceneWidget::addAxis: cannot add two horizontal or two vertical axes to a couple!");

    /* needed in case an axis is removed. The PlotChangeListener must be deinstalled
     */
    connect(scaleItem, SIGNAL(destroyed(QObject*)), this, SLOT(removeAxis(QObject*)));
    if(d->zoomer)
        d->zoomer->addScale(scaleItem);
    printf("add axis done scale item %p\n", scaleItem);
    return scaleItem;
}

void QGraphicsPlotItem::removeAxis(ScaleItem::Id id)
{
    Q_UNUSED(id);
}

/** \brief Removes the axes from the plot.
 *
 * @param all  true: all axes (horizontal and vertical) are removed
 * @param all false: only the axes with the specified orientation are removed
 *
 */
void QGraphicsPlotItem::clearAxes(bool all, ScaleItem::Orientation o)
{
    if(!d->axesManager)
        return;
    QList<ScaleItem *> axes = d->axesManager->getAxes(all, o);
    d->axesManager->clearAxes(all, o);
}

void QGraphicsPlotItem::clear()
{
    foreach(SceneCurve * c, getCurves())
        delete c;
    d->zoomer->clear();
    d->configurableObjectsMap.clear();
    clearAxes(false, ScaleItem::Vertical);
}

void QGraphicsPlotItem::resize(const QSizeF &s) {
    prepareGeometryChange();
    d->plotRect.setSize(s);
    update();
}

void QGraphicsPlotItem::setGeometry(const QRectF &r) {
    prepareGeometryChange();
    d->plotRect = r;
    update();
}

AxesManager *QGraphicsPlotItem::axesManager() const
{
    return d->axesManager;
}

void QGraphicsPlotItem::installPlotGeometryChangeListener(PlotGeometryEventListener *l)
{
    d->plotGeometryEventListeners.append(l);
}

void QGraphicsPlotItem::removePlotGeometryChangeListener(PlotGeometryEventListener *l)
{
    d->plotGeometryEventListeners.removeAll(l);
}

void QGraphicsPlotItem::installMouseEventListener(MouseEventListener *l)
{
    d->mouseEventListeners.append(l);
}

void QGraphicsPlotItem::removeMouseEventListener(MouseEventListener* l)
{
    d->mouseEventListeners.removeAll(l);
}

bool QGraphicsPlotItem::xScaleEnabled() const
{
    ScaleItem *xS = d->axesManager->getAxis(ScaleItem::xBottom);
    if(xS)
        return xS->isVisible();

    return false;
}

bool QGraphicsPlotItem::yScaleEnabled() const
{
    ScaleItem *yS = d->axesManager->getAxis(ScaleItem::yLeft);
    if(yS)
        return yS->isVisible();

    return false;
}

void QGraphicsPlotItem::setXScaleEnabled(bool en)
{
    ScaleItem *xS = d->axesManager->getAxis(ScaleItem::xBottom);
    if(xS)
        xS->setVisible(en);
    if(/*en && */xS)
        boundsChanged();
}

void QGraphicsPlotItem::setYScaleEnabled(bool en)
{
    ScaleItem *yS = d->axesManager->getAxis(ScaleItem::yLeft);
    if(yS)
        yS->setVisible(en);
    if(/*en && */yS)
        boundsChanged();
}

/** \brief returns the mouse zoom enable state
 *
 * @return true the user is able to zoom with the mouse (while the Control key
 *         is simultaneously pressed.
 * @return false the mouse zoomer is disabled
 *
 * @see setMouseZoomEnabled
 */
bool QGraphicsPlotItem::mouseZoomEnabled() const
{
    return d->mouseZoomEnabled;
}

/** \brief Enables the zoom with the mouse, when the Ctrl key and the left
 *         mouse button are pressed.
 *
 * @param en the Ctrl+Left mouse button zoomes the area drawn by the mouse
 * @param false the mouse zoomer is disabled.
 *
 * @see mouseZoomEnabled
 */
void QGraphicsPlotItem::setMouseZoomEnabled(bool en)
{
    d->mouseZoomEnabled = en;
}

/** \brief returns the zoom state
  *
  * @return true the plot is being zoomed
  * @return false the plot is not in zoom state
  *
  * The plot enters the zoom state when the user selects a rectangular area
  * with the left mouse button together with the Maiusc key.
  *
  * \par Note
  * The plot is not in zoom state when the user scrolls the mouse wheel to
  * scale the view.
  *
  */
bool QGraphicsPlotItem::inZoom() const
{
    return d->zoomer->inZoom();
}

/** \brief returns the zoom level of the zoomer.
  *
  * The minimum value returned by this call is 1 and it means that no zoom
  * is currently applied.
  * A value of 2 means that the plot has been zoomed 1 time, a value of 3
  * means that the plot has been zoomed 2 times and so on.
  *
  * @return the zoom level: 1 means no zoom, 2 means zoomed once and so on...
  */
int QGraphicsPlotItem::zoomLevel() const
{
    return d->zoomer->stackSize();
}


void QGraphicsPlotItem::setManualUpdate(bool manual) {
    QTimer *t = findChild<QTimer *>("refreshTimer");
    if(manual) {
        if(t)
            t->start();
    }
    else {
        /* stop the timer but do not delete it */
        if(t)
            t->stop();
    }
}

QRectF QGraphicsPlotItem::plotRect() const {
    return d->plotRect;
}

/** \brief returns the origin position (in percentage between 0.0 and 1.0) of the scale item
  *        passed as parameter
  *
  * Axes are placed with a position with respect to each other defined by this value,
  * expressed in percentage from 0.0 to 1.0.
  * By default, the axes have both 0.5 as position percentage value, meaning that
  * they intersect in the middle of each other.
  */
double QGraphicsPlotItem::originPosPercentage(ScaleItem *scaleIt) const
{
    if(!scaleIt)
        return 0.0;
    QList<AxisCouple> axCouples = d->axesManager->getAxisCouples();
    foreach(AxisCouple ac, axCouples)
    {
        switch(scaleIt->orientation())
        {
        case ScaleItem::Horizontal:
            if(ac.xAxis == scaleIt)
            {
                return ac.xOriginPosPercentage;
            }
            break;
        case ScaleItem::Vertical:
            if(ac.yAxis == scaleIt)
            {
                return ac.yOriginPosPercentage;
            }
            break;
        }
    }
    return -1.0;
}

/** \brief returns the  origin position (in percentage between 0.0 and 1.0) of the
  *        default x scale item (the one that the PlotSceneWidget always creates
  *        automatically)
  *
  *
  * Axes are placed with a position with respect to each other defined by this value,
  * expressed in percentage from 0.0 to 1.0.
  * By default, the axes have both 0.5 as position percentage value, meaning that
  * they intersect in the middle of each other.
  *
  * @see originPosPercentage
  * @see defaultYAxisOriginPosPercentage
  *
  */
double QGraphicsPlotItem::defaultXAxisOriginPosPercentage() const
{
    return originPosPercentage(this->xScaleItem());
}

/** \brief returns the  origin position (in percentage between 0.0 and 1.0) of the
  *        default y scale item (the one that the PlotSceneWidget always creates
  *        automatically)
  *
  * @see defaultXAxisOriginPosPercentage
  * @see originPosPercentage
  *
  */
double QGraphicsPlotItem::defaultYAxisOriginPosPercentage() const
{
    return originPosPercentage(this->yScaleItem());
}

/** \brief sets the position of the origin of the given scale item to the specified
  *        percent value, in percentage terms from 0 to 1.0
  *
  * @param scaleIt the scale item whose position you want to change
  * @param percent the value of the position, from 0 to 1.0.
  *
  * If you did not add additional axes by yourself, then you might want to use
  * the shortcut methods setDefaultXAxisOriginPosPercentage and setDefaultYAxisOriginPosPercentage.
  *
  * @see setDefaultXAxisOriginPosPercentage
  * @see setDefaultYAxisOriginPosPercentage
  */
void QGraphicsPlotItem::setOriginPosPercentage(ScaleItem *scaleIt, double percent)
{
    if(percent >=0 && percent <= 1.0)
    {
        for(int i = 0; i < d->axesManager->axes.size(); i++)
        {
            switch(scaleIt->orientation())
            {
            case ScaleItem::Horizontal:
                if(d->axesManager->axes[i].xAxis == scaleIt)
                {
                    d->axesManager->axes[i].xOriginPosPercentage = percent;
                    scaleIt->updateLabelsCache();
                    scaleIt->update();
                }
                break;
            case ScaleItem::Vertical:
                if(d->axesManager->axes[i].yAxis == scaleIt)
                {
                    d->axesManager->axes[i].yOriginPosPercentage = percent;
                    scaleIt->updateLabelsCache();
                    scaleIt->update();
                }
                break;
            }
        }
    }
}

/** \brief Changes the position of the default x axis.
  *
  * The default x axis is the axis that PlotSceneWidget always creates for you and
  * places at the position 0.5 by default.
  *
  * @param percent the value of the new position for the default x axis
  *
  * @see setOriginPosPercentage
  * @see setDefaultYAxisOriginPosPercentage
  *
  * If you added custom axes, then use the setOriginPosPercentage method instead.
  */
void QGraphicsPlotItem::setDefaultXAxisOriginPosPercentage(double percent)
{
    setOriginPosPercentage(this->xScaleItem(), percent);
}

/** \brief Changes the position of the default y axis.
  *
  * The default y axis is the axis that PlotSceneWidget always creates for you and
  * places at the position 0.5 by default.
  *
  * @param percent the value of the new position for the default y axis
  *
  * @see setOriginPosPercentage
  * @see setDefaultXAxisOriginPosPercentage
  *
  * If you added custom axes, then use the setOriginPosPercentage method instead.
  */
void QGraphicsPlotItem::setDefaultYAxisOriginPosPercentage(double percent)
{
    setOriginPosPercentage(this->yScaleItem(), percent);
}

/** \brief shortcut to get the axis bound from the xScaleItem()
 *
 * @return xScaleItem->lowerBound
 */
double QGraphicsPlotItem::xAxisLowerBound() const
{
    return xScaleItem()->lowerBound();
}

/** \brief shortcut to get the axis bound from the yScaleItem()
 *
 * @return yScaleItem->lowerBound
 *
 */
double QGraphicsPlotItem::yAxisLowerBound() const
{
    return yScaleItem()->lowerBound();
}

/** \brief shortcut to get the axis bound from the xScaleItem()
 *
 * @return xScaleItem->upperBound
 */
double QGraphicsPlotItem::xAxisUpperBound() const
{
    return xScaleItem()->upperBound();
}

/** \brief shortcut to get the axis bound from the yScaleItem()
 *
 * @return yScaleItem->upperBound
 */
double QGraphicsPlotItem::yAxisUpperBound() const
{
    return yScaleItem()->upperBound();
}

/** \brief shortcut to get the axis autoscale mode from the xScaleItem()
 *
 * @return xScaleItem()->axisAutoscaleEnabled()
 */
bool QGraphicsPlotItem::xAxisAutoscaleEnabled() const
{
    return xScaleItem()->axisAutoscaleEnabled();
}

/** \brief shortcut to get the axis autoscale mode from the yScaleItem()
 *
 * @return yScaleItem()->axisAutoscaleEnabled()
 */
bool QGraphicsPlotItem::yAxisAutoscaleEnabled() const
{
    return yScaleItem()->axisAutoscaleEnabled();
}

/** \brief Shortcut to access the x axis and set the bound.
 *
 */
void QGraphicsPlotItem::setXAxisLowerBound(double xlb)
{
    xScaleItem()->setLowerBound(xlb);
}

/** \brief Shortcut to access the y axis and set the bound.
 *
 */
void QGraphicsPlotItem::setYAxisLowerBound(double ylb)
{
    yScaleItem()->setLowerBound(ylb);
}

/** \brief Shortcut to access the x axis and set the bound.
 *
 */
void QGraphicsPlotItem::setXAxisUpperBound(double xub)
{
    xScaleItem()->setUpperBound(xub);
}

/** \brief Shortcut to access the y axis and set the bound.
 *
 */
void QGraphicsPlotItem::setYAxisUpperBound(double yub)
{
    yScaleItem()->setUpperBound(yub);
}

/** \brief Shortcut to enable/disable the autoscale on the x axis
 *
 * calls xScaleItem()->setAxisAutoscaleEnabled(en)
 */
void QGraphicsPlotItem::setXAxisAutoscaleEnabled(bool en)
{
    xScaleItem()->setAxisAutoscaleEnabled(en);
}

/** \brief Shortcut to enable/disable the autoscale on the x axis
 *
 * calls xScaleItem()->setAxisAutoscaleEnabled(en)
 */
void QGraphicsPlotItem::setYAxisAutoscaleEnabled(bool en)
{
    yScaleItem()->setAxisAutoscaleEnabled(en);
}

void QGraphicsPlotItem::setRefreshPeriod(int period)
{
    if(period > 0)
    {
        QTimer *t = findChild<QTimer *>("refreshTimer");
        if(!t)
            t = new QTimer(this);
        t->setObjectName("refreshTimer");
        t->setInterval(period);
        connect(t, SIGNAL(timeout()), scene(), SLOT(update()));
        t->start();
    }
    else
    {
        QTimer *t = findChild<QTimer *>("refreshTimer");
        if(t)
        {
            t->stop();
            delete t;
        }
    }
}

int QGraphicsPlotItem::refreshPeriod() const
{
    QTimer *t = findChild<QTimer *>("refreshTimer");
    if(t)
        return t->interval();
    else
        return -1;
}

/** \brief adds and configures a curve with a LinePainter
 *
 * This methods adds a new curve to the plot. The curve is represented in the plot by
 * lines joining its points.
 *
 * This method is equivalent to the calls to
 *
 * \code
 * SceneCurve *c = graphicsPlot->addCurve(name);
   CurveItem *curveItem = new CurveItem(c);
   graphicsPlot->scene()->addItem(curveItem);
   c->installCurveChangeListener(curveItem);
   LinePainter *lp = new LinePainter(curveItem);

 * \endcode
 * The colour is automatically chosen.
 */
SceneCurve *QGraphicsPlotItem::addLineCurve(const QString& name, ScaleItem *xScaleI, ScaleItem *yScaleI)
{
    if(!xScaleI)
        xScaleI = xScaleItem();
    if(!yScaleI)
        yScaleI = yScaleItem();

    ColorPalette colorPalette;
    /* create a curve */
    SceneCurve *sceneCurve = new SceneCurve(this, name, xScaleI, yScaleI);
    connect(sceneCurve, SIGNAL(destroyed(QObject*)), this, SLOT(curveAboutToBeDestroyed(QObject*)));
    xScaleI->installAxisChangeListener(sceneCurve);
    yScaleI->installAxisChangeListener(sceneCurve);
    /* curve item */
    CurveItem *curveItem = new CurveItem(sceneCurve, this);
    scene()->addItem(curveItem);
    curveItem->setObjectName(name);
    sceneCurve->installCurveChangeListener(curveItem);
    /* painter */
    LinePainter *lp = new LinePainter(curveItem);
    /* automatically pick a color */
    lp->setLineColor(colorPalette.getColor(d->curveHash.size()));
    lp->setObjectName(name);
    d->curveHash.insert(name, sceneCurve);
    emit curveAdded(sceneCurve);
    return sceneCurve;
}

void QGraphicsPlotItem::addCurve(SceneCurve *sceneCurve)
{
    ScaleItem *xScale = sceneCurve->getXAxis();
    ScaleItem *yScale = sceneCurve->getYAxis();
    d->curveHash.insert(sceneCurve->name(), sceneCurve);
    connect(sceneCurve, SIGNAL(destroyed(QObject*)), this, SLOT(curveAboutToBeDestroyed(QObject*)));
    xScale->installAxisChangeListener(sceneCurve);
    yScale->installAxisChangeListener(sceneCurve);
    emit curveAdded(sceneCurve);
}

SceneCurve *QGraphicsPlotItem::addCurve(const QString& name)
{
    return addCurve(name, xScaleItem(), yScaleItem());
}

SceneCurve *QGraphicsPlotItem::addCurve(const QString &name,
                                        ScaleItem* xScaleItem,
                                        ScaleItem* yScaleItem)
{
    if(xScaleItem && yScaleItem)
    {
        SceneCurve *sceneCurve = new SceneCurve(this, name, xScaleItem, yScaleItem);
        d->curveHash.insert(name, sceneCurve);
        /* if a curve is deleted by the user (instead of being removed via the removeCurve(QString) )
         * method, we have to manage the curve removal in a clean way.
         */
        connect(sceneCurve, SIGNAL(destroyed(QObject*)), this, SLOT(curveAboutToBeDestroyed(QObject*)));
        xScaleItem->installAxisChangeListener(sceneCurve);
        yScaleItem->installAxisChangeListener(sceneCurve);

        emit curveAdded(sceneCurve);

        return sceneCurve;
    }

    /* else: error */
    perr("PlotSceneWidget::addCurve: provided x scale item or y scale item (or both) is NULL");
    return NULL;
}

void QGraphicsPlotItem::curveAboutToBeDestroyed(const QString& name, bool deleteCurve)
{
    if(d->curveHash.contains(name))
    {
        SceneCurve *curve = d->curveHash.value(name);
        /* notify curve removal */
        emit curveAboutToBeRemoved(curve);

        curve->disconnect(); /* from the destroyed() signal in particular ;) */

        /* remove the curve from the axes (as AxisChangeListener ) */
        ScaleItem *xAxis = curve->getXAxis();
        ScaleItem *yAxis = curve->getYAxis();
        xAxis->removeAxisChangeListener(curve);
        yAxis->removeAxisChangeListener(curve);

        CurveItem *curveItem = curve->curveItem();
        if(curveItem)
        {
            /* QGraphicsScene::removeItem()
             * Qt: Removes the item item and all its children from the scene.
             * The ownership of item is passed
             * on to the caller (i.e., QGraphicsScene will no
             * longer delete item when destroyed).
             */
            scene()->removeItem(curveItem);
            qDebug() << __FUNCTION__ << "removed item " << curveItem->objectName();
            delete curveItem;
        }
        else
            perr("PlotSceneWidget::removeCurve: no curve item associated to \"%s\"", qstoc(name));
        d->curveHash.remove(name);
        if(deleteCurve)
            delete curve;
    }
    else
        perr("PlotSceneWidget::removeCurve: no curve with name \"%s\"", qstoc(name));
}

/** \brief this is a protected slot connected to the destroyed() signal of a SceneCurve
 *
 * This method is involved when the users calls delete on a SceneCurve instead of
 * using the removeCurve(const QString& name) method of the PlotSceneWidget, which is
 * the recommended way to remove a curve from the plot.
 */
void QGraphicsPlotItem::curveAboutToBeDestroyed(QObject *crv)
{
    SceneCurve *scrv = static_cast<SceneCurve *>(crv);
    if(scrv) /* cleanup, but do not delete (again) the curve */
        this->curveAboutToBeDestroyed(scrv->name(), false);
}

void QGraphicsPlotItem::removeAxis(QObject *_axis)
{
    ScaleItem* axis = static_cast<ScaleItem *>(_axis);
    this->removePlotGeometryChangeListener(axis);
    d->zoomer->removeScale(axis);
}


QList<SceneCurve *> QGraphicsPlotItem::getCurves() const
{
    return d->curveHash.values();
}

SceneCurve *QGraphicsPlotItem::findCurve(const QString& name)
{
    if(d->curveHash.contains(name))
        return d->curveHash.value(name);
    return NULL;
}

// return true if axis bounds changed
bool QGraphicsPlotItem::m_check_axis_bounds(SceneCurve *c) {
    bool bounds_changed = false;
    ScaleItem * xi = d->axesManager->getAxis(c->associatedXAxisId());
    ScaleItem * yi = d->axesManager->getAxis(c->associatedYAxisId());
    if(xi->axisAutoscaleEnabled() && !inZoom())
        bounds_changed |= xi->setBoundsFromCurves();
    if(yi->axisAutoscaleEnabled() && !inZoom())
        bounds_changed |= yi->setBoundsFromCurves();
    return bounds_changed;
}


void QGraphicsPlotItem::appendData(const QString& curveName, double x, double y)
{
    if(d->curveHash.contains(curveName))
    {
        SceneCurve *c = d->curveHash.value(curveName);
        QRectF r = c->addPoint(x, y); // scalar mode: updates max and min
        bool bch = m_check_axis_bounds(c); // bch -> bounds changed
        qDebug() << __PRETTY_FUNCTION__ << "updating area " << r << "bounds changed ?" << bch << "QRectF is" << QRectF();
        update(bch || r.isNull() ? QRectF() : r);
    }
    else
        perr("PlotSceneWidget: appendData: no curve with name \"%s\"", qstoc(curveName));
}

void QGraphicsPlotItem::appendData(const QString& curveName,
                                   const QVector<double>& xData,
                                   const QVector<double> &yData)
{
    if(d->curveHash.contains(curveName))
    {
        SceneCurve *c = d->curveHash.value(curveName);
        QRectF r = c->addPoints(xData, yData);
        qDebug() << __PRETTY_FUNCTION__ << "updating rect area " << r;
        bool fu = m_check_axis_bounds(c); // full update
        update(fu ? QRectF() : r);
    }
    else
        perr("PlotSceneWidget: appendData (vector version): no curve with name \"%s\"", qstoc(curveName));
}

void QGraphicsPlotItem::setData(const QString& curveName,
                                const QVector< double > &xData,
                                const QVector< double > &yData)
{
    if(d->curveHash.contains(curveName))
    {
        SceneCurve *c = d->curveHash.value(curveName);
        c->setData(xData, yData);
        update();
    }
    else
        perr("PlotSceneWidget: setData(): no curve with name \"%s\"", qstoc(curveName));
}

void QGraphicsPlotItem::setData(const QString& curveName,
                                const QVector< double > &yData)
{
    if(d->curveHash.contains(curveName))
    {
        SceneCurve *c = d->curveHash.value(curveName);
        c->setData(yData);
        update();
    }
    else
        perr("PlotSceneWidget: setData(yData): no curve with name \"%s\"", qstoc(curveName));
}

ScaleItem *QGraphicsPlotItem::xScaleItem() const
{
    return d->axesManager->getAxis(ScaleItem::xBottom);
}

ScaleItem *QGraphicsPlotItem::yScaleItem() const
{
    QList<ScaleItem *> allAxes = d->axesManager->getAllAxes();
    ScaleItem *y = d->axesManager->getAxis(ScaleItem::yLeft);
    return y;
}

ScaleItem* QGraphicsPlotItem::scaleItem(ScaleItem::Id id) const
{
    return d->axesManager->getAxis(id);
}

ScaleItem* QGraphicsPlotItem::associatedAxis(ScaleItem::Id otherAxisId) const
{
    return d->axesManager->getAssociatedAxis(otherAxisId);
}

QPair<double, double> QGraphicsPlotItem::associatedOriginPosPercentage(ScaleItem::Id xAxisId, ScaleItem::Id yAxisId, bool *ok) const
{
    return d->axesManager->getOriginPosPercentage(xAxisId, yAxisId, ok);
}

QList<SceneCurve *> QGraphicsPlotItem::curvesForAxes(ScaleItem::Id axisId, ScaleItem::Orientation orientation) const
{
    QList<SceneCurve *> curves;
    ScaleItem::Id aId;
    foreach(SceneCurve *sc, d->curveHash.values())
    {
        if(orientation == ScaleItem::Horizontal)
            aId = sc->associatedXAxisId();
        else
            aId = sc->associatedYAxisId();

        if(aId == axisId)
            curves << sc;
    }
    return curves;
}

void QGraphicsPlotItem::boundsChanged()
{
    foreach(SceneCurve *sc, d->curveHash.values())
        sc->invalidateCache();
    /* redraw all the axis */
    foreach(ScaleItem* scaleItem, d->axesManager->getAllAxes())
        scaleItem->update();
}

QVariant QGraphicsPlotItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    switch(change) {
    case QGraphicsItem::ItemVisibleChange:
        break;
    default:
        break;
    }
    return QGraphicsItem::itemChange(change, value);
}


void QGraphicsPlotItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        d->mousePressed = true;

    if(event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)
    {
        d->mouseMoving = true;
        d->mousePressedPoint =  event->pos();
    }
    else if(event->button() == Qt::LeftButton && event->modifiers() & Qt::ControlModifier)
        mSwitchAxesCurvesForeground();
    else if(!(event->modifiers() & Qt::ShiftModifier))
        ; // setDragMode(QGraphicsView::ScrollHandDrag);

    foreach(MouseEventListener* l, d->mouseEventListeners)
        l->mousePressEvent(this, event);
}

void QGraphicsPlotItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    foreach(MouseEventListener* l, d->mouseEventListeners)
        l->mouseReleaseEvent(this, event);

    if(d->mousePressed && event->button() == Qt::LeftButton)
    {
#if QT_VERSION < 0x050000
        foreach(MouseEventListener *l, d_ptr->mouseEventListeners)
            l->mouseClickEvent(this, event->posF());

        emit clicked(event->posF());
#else
        foreach(MouseEventListener *l, d->mouseEventListeners)
            l->mouseClickEvent(this, event->pos());

        emit clicked(event->pos());
#endif
        d->mousePressed = false;
    }
    if(d->mouseMoving) /* release after a move with left button pressed */
    {
        d->mouseMoving = false;
        d->mouseMovingPoint = event->pos();

        if(d->mouseZoomEnabled)
        {
            QPointF tmpPoint;
            QPointF mP1 = d->mousePressedPoint;
            QPointF mP2 = d->mouseMovingPoint;
            if(mP1.x() > mP2.x()) /* swap points */
            {
                tmpPoint = mP1;
                mP1.setX(mP2.x());
                mP2.setX(tmpPoint.x());
            }
            if(mP1.y() < mP2.y()) /* swap points */
            {
                tmpPoint = mP1;
                mP1.setY(mP2.y());
                mP2.setY(tmpPoint.y());
            }
            QRectF selectionRect = QRectF(mP1, mP2);
            d->zoomer->zoom(selectionRect);
            d->zoomArea = QRectF();
        }
        //update(); /* shouldn't be necessary */
    }
    if(event->button() == Qt::MidButton)
    {
        if(d->mouseZoomEnabled)
        {
            d->zoomer->unzoom();
        }
    }
}

void QGraphicsPlotItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    foreach(MouseEventListener *l, d->mouseEventListeners)
        l->mouseDoubleClickEvent(this, event);
}

void QGraphicsPlotItem::mSwitchAxesCurvesForeground()
{
    //    double xZVal = xScaleItem()->zValue();
    //    double yZVal = yScaleItem()->zValue();
    //    if(xZVal < 1000 && yZVal < 1000)
    //    {
    //        xScaleItem()->setZValue(1000);
    //        yScaleItem()->setZValue(1000);
    //    }
    //    else
    //    {
    //        xScaleItem()->setZValue(0);
    //        yScaleItem()->setZValue(0);
    //    }

}

void QGraphicsPlotItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    foreach(MouseEventListener *l, d->mouseEventListeners)
        l->mouseMoveEvent(this, event);

    if(d->mouseMoving && d->mouseZoomEnabled)
    {
        d->mouseMoving = true;
        ///  d_ptr->mouseMovingPoint = mapToScene(event->pos());
        d->mouseMovingPoint = event->pos();

        /* let the scene draw the zoom rect with a green dashed line */
        QPointF topLeft, botRight;
        if(d->mouseMovingPoint.x() > d->mousePressedPoint.x())
        {
            topLeft.setX(d->mousePressedPoint.x());
            botRight.setX(d->mouseMovingPoint.x());
        }
        else
        {
            topLeft.setX(d->mouseMovingPoint.x());
            botRight.setX(d->mousePressedPoint.x());
        }
        if(d->mouseMovingPoint.y() > d->mousePressedPoint.y())
        {
            topLeft.setY(d->mousePressedPoint.y());
            botRight.setY(d->mouseMovingPoint.y());
        }
        else
        {
            topLeft.setY(d->mouseMovingPoint.y());
            botRight.setY(d->mousePressedPoint.y());
        }
        d->zoomArea = QRectF(topLeft, botRight);
        update();
    }
    if(d->mousePressed)
        d->mousePressed = false;
}

void QGraphicsPlotItem::update(const QRectF& area) {
    d->updateRect = area;
    QGraphicsObject::update(area);
}

void QGraphicsPlotItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(d->zoomArea.isValid()) {
        QPen zoomAreaPen(Qt::gray);
        zoomAreaPen.setStyle(Qt::DashLine);
        painter->setPen(zoomAreaPen);
        painter->drawRect(d->zoomArea);
    }
    QPen p(Qt::darkYellow);
    QBrush b(Qt::yellow);
    painter->setBrush(b);
    painter->setPen(p);
    painter->drawRect(d->updateRect);
    if(d->backgroundColor.isValid()) {
        painter->setBrush(QBrush(d->backgroundColor));
        painter->drawRect(d->plotRect);
    }
}

double QGraphicsPlotItem::transform(const double x, ScaleItem* scaleItem) const {
    double coord = 0.0;
    if(scaleItem)
    {
        double len;
        double start = scaleItem->lowerBound();
        double end = scaleItem->upperBound();
        if(start < end)
        {
            if(scaleItem->orientation() == ScaleItem::Horizontal)
            {
                double pas = scaleItem->plotArea().x(); // plot area start, x
                len = scaleItem->plotAreaW - 1 ;
                coord = (x - start) * len / (end - start) + pas;
                double x1 = start * len / (end - start) + pas;
                double x2 = end * len / (end - start) + pas;

                printf("\e[1;33mcoord of value %f is %f \e[0m\n", x, coord);
                printf("\e[1;36mlb %f maps to %f ub %f maps to %f -- plot area starts at %f\e[0m\n",
                       start, x1, end, x2, pas);
                printf("\e[1;32m [ %f < %f < %f ]\e[0m\n", x1, coord, x2);
                qDebug() << "          plot area " << scaleItem->plotArea();
            }
            else
            {
                /* -1 because when x == start the coord must be the canvasHeight - 1
                 * in order to be drawn (coordinates start from 0).
                 * When x == end the coordinate is 0, which is correct.
                 * In practise, the extension must be [0 - scaleItem->canvasHeight - 1].
                 */
                len = scaleItem->plotAreaH - 1;
                coord = len - (x - start) * len / (end - start);
                //                qDebug() << __FUNCTION__ << coord << " len " << len << " x " << x << "start" <<
                //                            start << "end" << end << " end - start" << end - start;
            }
        }
    }
    else
        perr("PlotSceneWidget::transform: invalid axis (NULL)");
    printf("QGraphicsPlotItem::transform \e[1;35m%f --> %f\e[0m\n", x, coord);
    return coord;
}

/** \brief transforms a QPointF in scene coordinates into a QPointF containing the
  *        x and y values in the x and y axis scale coordinates.
  *
  * This method operates on the default x and y scale items, the ones returned by
  * xScaleItem and yScaleItem. If you want to transform according to other scale items
  * use the invTransform(const double pt, ScaleItem* scaleItem) method instead.
  *
  * @see invTransform(const double pt, ScaleItem* scaleItem) const
  * @see transform
  *
  */
QPointF QGraphicsPlotItem::invTransform(const QPointF& pSceneCoord)
{
    if(xScaleItem() && yScaleItem())
        return invTransform(pSceneCoord, xScaleItem(), yScaleItem());

    perr("PlotSceneWidget::invTransform(const QPointF& pSceneCoord): invalid axes");
    return QPointF();
}


/** \brief transforms a double in scene coordinates into a double in scaleItem coordinates
  *
  * @param pt the x or y coordinate of the point you want to transform, in scene coordinates
  * @param scaleItem the ScaleItem on which to calculate the transformed value.
  * @return the value of the transformed point, in the scaleItem coordinates.
  *
  * @see invTransform(const double pt, ScaleItem* scaleItem) const
  * @see transform
  *
  */
double QGraphicsPlotItem::invTransform(const double pt, ScaleItem* scaleItem) const
{
    if(scaleItem && scaleItem->orientation() == ScaleItem::Horizontal)
        return scaleItem->lowerBound() + pt * (scaleItem->upperBound() - scaleItem->lowerBound()) / scaleItem->plotAreaW;
    else if(scaleItem && scaleItem->orientation() == ScaleItem::Vertical)
        return scaleItem->upperBound() - pt * (scaleItem->upperBound() - scaleItem->lowerBound()) / scaleItem->plotAreaH;

    perr("PlotSceneWidget::invTransform: invalid scale item (NULL)");
    return 0;
}

QPointF QGraphicsPlotItem::invTransform(const QPointF& pSceneCoord, ScaleItem* xScaleItem, ScaleItem* yScaleItem) const
{
    if(xScaleItem && yScaleItem)
    {
        ScaleItem *xScI = xScaleItem;
        ScaleItem *yScI = yScaleItem;
        qreal px = pSceneCoord.x();
        qreal py = pSceneCoord.y();

        qreal x = xScI->lowerBound() + px * (xScI->upperBound() - xScI->lowerBound()) / xScaleItem->plotAreaW;
        qreal y = yScI->upperBound() - py * (yScI->upperBound() - yScI->lowerBound()) / yScaleItem->plotAreaH;

        //        qDebug() << __FUNCTION__ << xScaleItem << yScaleItem << px << py <<xScaleItem->canvasWidth
        //                 << yScaleItem->canvasHeight << x << y;

        return QPointF(x, y);
    }
    perr("PlotSceneWidget::invTransform(const QPointF&, ScaleItem*, ScaleItem*): invalid axes");
    return QPointF();
}

QList<SceneCurve*> QGraphicsPlotItem::getClosest(
        QPointF &closestPos,
        int *closestIndex,
        const QPointF& pos)
{
    QList<SceneCurve *> ret;
    closestPos = QPointF();
    SceneCurve *closestCurve = NULL;
    *closestIndex = -1;
    double minDist = 1e9, dist, xp, yp, xc, yc;
    foreach(SceneCurve *c, d->curveHash.values())
    {
        const QPointF *points = c->points();
        int dataSize = c->data()->size();
        for(int i = 0; i < dataSize; i++)
        {
            xc = points[i].x();
            yc = points[i].y();
            xp = pos.x();
            yp = pos.y();
            dist = sqrt(pow(xp - xc, 2) + pow(yp - yc, 2));
            //   qDebug() << "dist " << dist << "min dist" << minDist;
            if(dist < minDist)
            {
                minDist = dist;
                closestCurve = c;
                *closestIndex = i;
                closestPos = points[i];
            }
        }
    }
    if(closestCurve)
    {
        ret << closestCurve;
        /* test whether there are overlapping curves in that point */
        double x = closestCurve->data()->xData.at(*closestIndex);
        double y = closestCurve->data()->yData.at(*closestIndex);
        double otherx, othery;
        foreach(SceneCurve *c, d->curveHash.values())
        {
            if(c != closestCurve && c)
            {
                Data *data = c->data();
                if(c->dataSize() > *closestIndex)
                {
                    otherx = data->xData.at(*closestIndex);
                    othery = data->yData.at(*closestIndex);
                    /* if x and y at closestIndex are the same, add the curve */
                    if(otherx == x &&
                            (othery == y || (isnan(othery) && isnan(y)) ) )
                    {
                        ret << c;
                    }/*
                    else
                        qDebug() << "not retting " << c->name() << "cuz " <<
                                   ( c->data()->xData.at(*closestIndex)  == x)
                                 << (c->data()->yData.at(*closestIndex) == y)<<
                                    c->data()->xData.at(*closestIndex) << x
                                  << c->data()->yData.at(*closestIndex) << y;*/
                }
            }
        }
    }
    return ret;
}

void QGraphicsPlotItem::loadConfigurationProperties()
{
    SettingsLoader settingsLoader(d->settingsKey);
    foreach(QObject *object, d->configurableObjectsMap.values())
        settingsLoader.loadConfiguration(object);
}

//void QGraphicsPlotItem::setSettingsKey(const QString& key)
//{
//    d->settingsKey = key;
//}

//QString QGraphicsPlotItem::settingsKey() const
//{
//    return d->settingsKey;
//}

/** \brief add an object which is configurable through its Qt properties.
  *
  * @param title: in the configuration dialog of the plot the object properties
  *        will appear under a tab widget with title as the title page.
  *
  * @param object the object whose properties are selected and configurable.
  *        May be a widget for example, or a graphics object item.
  *
  * @see removeConfigurableObjects
  */
void QGraphicsPlotItem::addConfigurableObjects(const QString& title, QObject* object)
{
    d->configurableObjectsMap.insert(title, object);
}

/** \brief removes an object which was previously added with addConfigurableObjects
 *         with the given title.
  *
  * @param title: in the configuration dialog of the plot the object properties
  *        will appear under a tab widget with title as the title page.
  *
  * @see addConfigurableObjects
  */
void QGraphicsPlotItem::removeConfigurableObjects(const QString& title) {
    d->configurableObjectsMap.remove(title);
}

QMenu * QGraphicsPlotItem::createContextMenu() {
    d->menu = new QMenu(0);
    d->menu->addAction("Configure...", this, SLOT(executePropertyDialog()));
    QAction *fitInViewAction = d->menu->addAction("Fit in view", this, SLOT(fitIn()));
    fitInViewAction->setToolTip("Fits the plot in the view");
    QAction *resetAction = d->menu->addAction("Reset Transform", this, SLOT(resetTransformMatrix()));
    resetAction->setToolTip("Reset the transform matrix,\n"
                            "i.e. reset the scale factor to 1:1");
    d->menu->addSeparator();
    QAction *saveDataAction = d->menu->addAction("Save Data...", this, SLOT(saveData()));
    saveDataAction->setToolTip("Open a dialog to save data on a file with different format options");
    return d->menu;
}

void QGraphicsPlotItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *)
{
    if(!findChild<PropertyDialog *>())
    {
        QMenu *menu = createContextMenu();
        menu->exec(QCursor::pos());
        delete menu;
    }
}

void QGraphicsPlotItem::executePropertyDialog()
{
    PropertyDialog *propertyDialog = new PropertyDialog(this);
    propertyDialog->setAttribute(Qt::WA_DeleteOnClose);
    propertyDialog->configureTabs(d->configurableObjectsMap);
    propertyDialog->setSettingsKey(d->settingsKey);
    propertyDialog->show();
}

LegendItem *QGraphicsPlotItem::legendItem() const
{
    return d->legendItem;
}

QGraphicsZoomer *QGraphicsPlotItem::zoomer() const
{
    return d->zoomer;
}

void QGraphicsPlotItem::setLegendVisible(bool visible)
{
    d->legendItem->setVisible(visible);
}

bool QGraphicsPlotItem::legendVisible() const
{
    return d->legendItem->isVisible();
}

void QGraphicsPlotItem::setBackgroundColor(const QColor& c) const {
    d->backgroundColor = c;
}

QColor QGraphicsPlotItem::backgroundColor() const
{
    return d->backgroundColor;
}

void QGraphicsPlotItem::saveData()
{
    QList<SceneCurve *> curves = this->getCurves();
    bool timeScale = false;
    if(curves.size())
    {
        ScaleLabelInterface *scaleLabelInterface = xScaleItem()->scaleLabelInterface();
        timeScale = (scaleLabelInterface && scaleLabelInterface->type() == ScaleLabelInterface::TimeScale);
        PlotSceneWidgetSaver saver;
        if(!saver.save(curves, timeScale))
            QMessageBox::critical(0, "Error saving on file", QString("Error saving file \"%1\":\n%2").arg(saver.fileName()).
                                  arg(saver.errorMessage()));
    }
}

QRectF QGraphicsPlotItem::boundingRect() const {
    return d->plotRect;
}
