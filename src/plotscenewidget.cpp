#include "plotscenewidget.h"
#include "curve/point.h"
#include "curve/pointdata.h"
#include "qgraphicsplotmacros.h"
#include "plotscenewidget_private.h"
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
#include "graphicsscene.h"
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
PlotSceneWidget::PlotSceneWidget(QWidget *parent, bool useOpenGl) :
    QGraphicsView(parent)
{
    initPlot(useOpenGl);
    initDefaultAxes();
}


PlotSceneWidget::PlotSceneWidget(QWidget *parent, bool initDefAxes, bool useOpenGl):
    QGraphicsView(parent)
{
    initPlot(useOpenGl);
    if(initDefAxes)
        initDefaultAxes();
}

void PlotSceneWidget::initPlot(bool useOpenGl)
{
    qRegisterMetaType<PointData>("PointData");
    qRegisterMetaType<QVector<PointData> >("QVector<PointData>&");
    d_ptr = new PlotSceneWidgetPrivate(this);
    d_ptr->axesManager = NULL;
    if(useOpenGl || qApp->arguments().contains("--use-gl"))
    {
        d_ptr->useGl = true;
        QGLWidget *glWidget = new QGLWidget(QGLFormat(QGL::SampleBuffers));
        glWidget->makeCurrent();
        this->setViewport(glWidget);
        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        printf("\e[1;32m* \e[0musing openGL\n");
    }

    GraphicsScene *scene = new GraphicsScene(this);
    /* set a scene rect because ScaleItem needs a fixed scene rect.
     * The scene rect will be updated on showEvent
     */
    scene->setSceneRect(0, 0, 500, 400);
    setScene(scene);
    setMouseTracking(true);

    setRenderHint(QPainter::Antialiasing);
    setCursor(QCursor(Qt::CrossCursor));
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHints( QPainter::SmoothPixmapTransform);

    d_ptr->scrollBarsEnabled = true;

    /* zoom manager */
    d_ptr->zoomer = new QGraphicsZoomer(this);

    /* for each key of the map, create a tab page with all the widgets properties */
    addConfigurableObjects("Plot", this);

    connect(scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));

    /* when the user scrolls the view, immediately update the scene without waiting for the
     * scene updates deriving from curve updates or from other timer updates.
     */
    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), scene, SLOT(update()));
    connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)), scene, SLOT(update()));

    /* this is to notify PlotEventListener classes that the scroll bar values have changed.
     * For instance, an ExternalScaleWidget must be informed when a scroll bar value changes.
     */
    connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(hScrollBarValueChanged(int)));

    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(vScrollBarValueChanged(int)));

    /* legend */
    d_ptr->legendItem = new LegendItem(scene);
    /* set it outside the scene rect until it becomes visible */
    d_ptr->legendItem->setPos(sceneRect().width(), 5);
    connect(this, SIGNAL(curveAdded(SceneCurve*)), d_ptr->legendItem, SLOT(newCurveAdded(SceneCurve*)));
    connect(this, SIGNAL(curveAboutToBeRemoved(SceneCurve*)), d_ptr->legendItem, SLOT(curveRemoved(SceneCurve*)));
    d_ptr->legendItem->setVisible(false);
    d_ptr->legendItem->setObjectName("PlotSceneWidgetLegendItem");
    scene->addItem(d_ptr->legendItem);
    addConfigurableObjects("Legend", d_ptr->legendItem);
}

void PlotSceneWidget::initDefaultAxes()
{
    /* x and y scales enabled by default: create them */
    d_ptr->axesManager = new AxesManager();
    ScaleItem *xScaleItem = addAxis(ScaleItem::Horizontal, ScaleItem::xBottom, NULL);
    ScaleItem *yScaleItem = addAxis(ScaleItem::Vertical,  ScaleItem::yLeft, xScaleItem);

    addConfigurableObjects("X Axis", xScaleItem);
    addConfigurableObjects("Y Axis", yScaleItem);

    setXScaleEnabled(true); /* shows and positions them */
    setYScaleEnabled(true);
}

ScaleItem* PlotSceneWidget::addAxis(ScaleItem::Orientation o, ScaleItem::Id id, ScaleItem* associatedAxis)
{
    if(!d_ptr->axesManager)
        d_ptr->axesManager = new AxesManager();

    AxisCouple *co = d_ptr->axesManager->findIncompleteCouple(associatedAxis);
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
        d_ptr->axesManager->addCouple(AxisCouple(scaleItem, associatedAxis, 0.5, 0.5));
    }
    else if(!co && o == ScaleItem::Vertical
            && (associatedAxis == NULL || associatedAxis->orientation() == ScaleItem::Horizontal)) /* add a new axis couple */
    {
        d_ptr->axesManager->addCouple(AxisCouple(associatedAxis, scaleItem, 0.5, 0.5));
    }
    else
        perr("PlotSceneWidget::addAxis: cannot add two horizontal or two vertical axes to a couple!");

    this->scene()->addItem(scaleItem);
    /* needed in case an axis is removed. The PlotChangeListener must be deinstalled
     */
    connect(scaleItem, SIGNAL(destroyed(QObject*)), this, SLOT(removeAxis(QObject*)));
    if(d_ptr->zoomer)
        d_ptr->zoomer->addScale(scaleItem);
    return scaleItem;
}

void PlotSceneWidget::removeAxis(ScaleItem::Id id)
{
    Q_UNUSED(id);
}

/** \brief Removes the axes from the plot.
 *
 * @param all  true: all axes (horizontal and vertical) are removed
 * @param all false: only the axes with the specified orientation are removed
 *
 */
void PlotSceneWidget::clearAxes(bool all, ScaleItem::Orientation o)
{
    if(!d_ptr->axesManager)
        return;
    QList<ScaleItem *> axes = d_ptr->axesManager->getAxes(all, o);
    d_ptr->axesManager->clearAxes(all, o);
}

void PlotSceneWidget::clear()
{
    foreach(SceneCurve * c, getCurves())
        delete c;
    d_ptr->zoomer->clear();
    d_ptr->configurableObjectsMap.clear();
    clearAxes(false, ScaleItem::Vertical);
}

AxesManager *PlotSceneWidget::axesManager() const
{
    return d_ptr->axesManager;
}

void PlotSceneWidget::setPainterAntiAlias(bool en)
{
    setRenderHint(QPainter::Antialiasing, en);
}

bool PlotSceneWidget::painterAntiAlias() const
{
    return this->renderHints() & QPainter::Antialiasing;
}

void PlotSceneWidget::setPainterHQGLAntiAlias(bool en)
{
    setRenderHint(QPainter::HighQualityAntialiasing, en);
}

void PlotSceneWidget::setSmoothPixmapTransform(bool enable)
{
    setRenderHint(QPainter::SmoothPixmapTransform, enable);
}

bool PlotSceneWidget::smoothPixmapTransform() const
{
    return renderHints() & QPainter::SmoothPixmapTransform;
}

bool PlotSceneWidget::scrollBarsEnabled() const
{
    return d_ptr->scrollBarsEnabled;
}

void PlotSceneWidget::setScrollBarsEnabled(bool en)
{
    d_ptr->scrollBarsEnabled = en;
    if(!en)
    {
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else
    {
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}

void PlotSceneWidget::installPlotGeometryChangeListener(PlotGeometryEventListener *l)
{
    d_ptr->plotGeometryEventListeners.append(l);
}

void PlotSceneWidget::removePlotGeometryChangeListener(PlotGeometryEventListener *l)
{
    d_ptr->plotGeometryEventListeners.removeAll(l);
}

void PlotSceneWidget::installMouseEventListener(MouseEventListener *l)
{
    d_ptr->mouseEventListeners.append(l);
}

void PlotSceneWidget::removeMouseEventListener(MouseEventListener* l)
{
    d_ptr->mouseEventListeners.removeAll(l);
}

bool PlotSceneWidget::painterHQGLAntiAlias() const
{
    return this->renderHints() & QPainter::HighQualityAntialiasing;
}

bool PlotSceneWidget::xScaleEnabled() const
{
    ScaleItem *xS = d_ptr->axesManager->getAxis(ScaleItem::xBottom);
    if(xS)
        return xS->isVisible();

    return false;
}

bool PlotSceneWidget::yScaleEnabled() const
{
    ScaleItem *yS = d_ptr->axesManager->getAxis(ScaleItem::yLeft);
    if(yS)
        return yS->isVisible();

    return false;
}

void PlotSceneWidget::setXScaleEnabled(bool en)
{
    ScaleItem *xS = d_ptr->axesManager->getAxis(ScaleItem::xBottom);
    if(xS)
        xS->setVisible(en);
    if(en && xS)
        boundsChanged();
}

void PlotSceneWidget::setYScaleEnabled(bool en)
{
    ScaleItem *yS = d_ptr->axesManager->getAxis(ScaleItem::yLeft);
    if(yS)
        yS->setVisible(en);
    if(en && yS)
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
bool PlotSceneWidget::mouseZoomEnabled() const
{
    return d_ptr->mouseZoomEnabled;
}

/** \brief Enables the zoom with the mouse, when the Ctrl key and the left
 *         mouse button are pressed.
 *
 * @param en the Ctrl+Left mouse button zoomes the area drawn by the mouse
 * @param false the mouse zoomer is disabled.
 *
 * @see mouseZoomEnabled
 */
void PlotSceneWidget::setMouseZoomEnabled(bool en)
{
    d_ptr->mouseZoomEnabled = en;
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
bool PlotSceneWidget::inZoom() const
{
    return d_ptr->zoomer->inZoom();
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
int PlotSceneWidget::zoomLevel() const
{
    return d_ptr->zoomer->stackSize();
}

/** \brief Changes the scaleSceneOnResizeEnabled property value
  *
  * Read setScaleSceneOnResizeEnabled documentation for details.
  *
  * @param en true the scene is resized when the PlotSceneWidget is resized, until
  *        the user scrolls on the plot to scale the scene or the scene is scaled
  *        programmatically.
  *
  * @param en false the scene is not fitted in the view when the view (i.e. PlotSceneWidget)
  *        is resized.
  *
  * @see scaleSceneOnResizeEnabled
  */
void PlotSceneWidget::setScaleSceneOnResizeEnabled(bool en)
{
    d_ptr->scaleOnResize = en;
}

/** \brief returns the boolean value of the scaleSceneOnResizeEnabled property.
  *
  * If this property is set to true, then the plot is fitted on the view each time
  * the PlotSceneWidget is resized.
  * In other words, the scene rect is set to the viewport rect each time the widget is
  * resized.
  *
  * \par Note
  * The behaviour describe above stops being applied after that the user zooms on the plot
  * or uses the mouse wheel to alter the view transform (scales the view).
  * After a zoom or a manual scaling, the user manages the scaling of the view manually
  * until next application restart.
  *
  * \par Note
  * When part of a QTabWidget, it is recommended that this property is enabled, since
  * the geometry of the QTabWidget children is not correctly provided on the first show
  * event.
  *
  * \par Note
  * By default, this property is enabled in releases following the  2.2.1.
  *
  * @see setScaleSceneOnResizeEnabled
  */
bool PlotSceneWidget::scaleSceneOnResizeEnabled() const
{
    return d_ptr->scaleOnResize;
}

void PlotSceneWidget::setScaleSceneOnMouseScroll(bool en)
{
    d_ptr->scaleOnScroll    = en;
}

bool PlotSceneWidget::scaleSceneOnMouseScroll() const
{
    return d_ptr->scaleOnScroll;
}

void PlotSceneWidget::setSceneRectToWidgetGeometry(bool en)
{
    d_ptr->sceneRectToWidgetGeometry = en;
}

bool PlotSceneWidget::sceneRectToWidgetGeometry() const
{
    return d_ptr->sceneRectToWidgetGeometry;
}

bool PlotSceneWidget::modifiedPaintEvent() const
{
    return d_ptr->modifiedPaintEvent;
}

void PlotSceneWidget::setModifiedPaintEvent(bool mod)
{
    d_ptr->modifiedPaintEvent = mod;
}

void PlotSceneWidget::recalculatePlotRect()
{
    double x, y, w, h;
    QRectF sr = scene()->sceneRect();
    x = sr.left() + sr.width() * d_ptr->topLeftXPercent;
    y = sr.top() + sr.height() * d_ptr->topLeftYPercent;
    w = sr.width() * d_ptr->widthPercent;
    h = sr.height() * d_ptr->heightPercent;
    d_ptr->plotRect = QRectF(x, y, w, h);

    foreach(PlotGeometryEventListener *listener, d_ptr->plotGeometryEventListeners)
        listener->plotRectChanged(d_ptr->plotRect);

    /* emit signals to notify rect and size change
     */
    emit plotRectChanged(d_ptr->plotRect);
    /* this takes into account the plotRect and the QTransform
     * currently applied to emit a size changed signal.
     */
    notifyPlotAreaChanged();
}

QRectF PlotSceneWidget::plotRect() const
{
    return d_ptr->plotRect;
}

double PlotSceneWidget::plotAreaPercentageWidth() const
{
    return d_ptr->widthPercent;
}

double PlotSceneWidget::plotAreaPercentageHeight() const
{
    return d_ptr->heightPercent;
}

double PlotSceneWidget::plotAreaTopLeftXPercentage() const
{
    return d_ptr->topLeftXPercent;
}

double PlotSceneWidget::plotAreaTopLeftYPercentage () const
{
    return d_ptr->topLeftYPercent;
}

void PlotSceneWidget::setPlotAreaPercentageWidth(double widthPercent)
{
    setPlotAreaPercentage(widthPercent, d_ptr->heightPercent);
}

void PlotSceneWidget::setPlotAreaPercentageHeight(double heightPercent)
{
    setPlotAreaPercentage(d_ptr->widthPercent, heightPercent);
}

void PlotSceneWidget::setPlotAreaTopLeftXPercentage(double xPercent)
{
    setPlotAreaTopLeftPercentage(xPercent, d_ptr->topLeftYPercent);
}

void PlotSceneWidget::setPlotAreaTopLeftYPercentage(double yPercent)
{
    setPlotAreaTopLeftPercentage(d_ptr->topLeftXPercent, yPercent);
}

void PlotSceneWidget::setPlotAreaTopLeftPercentage(double widthPercent, double heightPercent)
{
    if(widthPercent >= 0 && widthPercent < 1 && heightPercent >= 0 && heightPercent < 1)
    {
        d_ptr->topLeftXPercent = widthPercent;
        d_ptr->topLeftYPercent = heightPercent;
        recalculatePlotRect();
        boundsChanged();
        scene()->update();
    }
    else
        perr("PlotSceneWidget::setPlotAreaSTopLeftPercentage: invalid values: must be between 0 and 1");
}

void PlotSceneWidget::setPlotAreaPercentage(double widthPercent, double heightPercent)
{
    if(widthPercent > 0 && widthPercent <= 1 && heightPercent > 0 && heightPercent <= 1)
    {
        d_ptr->widthPercent = widthPercent;
        d_ptr->heightPercent = heightPercent;
        recalculatePlotRect();
        boundsChanged();
        scene()->update();
    }
    else
        perr("PlotSceneWidget::setPlotAreaPercentage: invalid values: must be between 0 and 1");
}

void PlotSceneWidget::scale(qreal sx, qreal sy)
{
    QGraphicsView::scale(sx, sy);
//    qDebug() << "scale: sceneRect" << this->sceneRect() << "scene->sceneRect" <<
//                scene()->sceneRect();
//    qDebug() << QGraphicsView::transform();
    notifyPlotAreaChanged();
}

/** \brief returns the origin position (in percentage between 0.0 and 1.0) of the scale item
  *        passed as parameter
  *
  * Axes are placed with a position with respect to each other defined by this value,
  * expressed in percentage from 0.0 to 1.0.
  * By default, the axes have both 0.5 as position percentage value, meaning that
  * they intersect in the middle of each other.
  */
double PlotSceneWidget::originPosPercentage(ScaleItem *scaleIt) const
{
    if(!scaleIt)
        return 0.0;
    QList<AxisCouple> axCouples = d_ptr->axesManager->getAxisCouples();
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
double PlotSceneWidget::defaultXAxisOriginPosPercentage() const
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
double PlotSceneWidget::defaultYAxisOriginPosPercentage() const
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
void PlotSceneWidget::setOriginPosPercentage(ScaleItem *scaleIt, double percent)
{
    if(percent >=0 && percent <= 1.0)
    {
        for(int i = 0; i < d_ptr->axesManager->axes.size(); i++)
        {
            switch(scaleIt->orientation())
            {
            case ScaleItem::Horizontal:
                if(d_ptr->axesManager->axes[i].xAxis == scaleIt)
                {
                    d_ptr->axesManager->axes[i].xOriginPosPercentage = percent;
                    scaleIt->updateLabelsCache();
                    scaleIt->update();
                }
                break;
            case ScaleItem::Vertical:
                if(d_ptr->axesManager->axes[i].yAxis == scaleIt)
                {
                    d_ptr->axesManager->axes[i].yOriginPosPercentage = percent;
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
void PlotSceneWidget::setDefaultXAxisOriginPosPercentage(double percent)
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
void PlotSceneWidget::setDefaultYAxisOriginPosPercentage(double percent)
{
    setOriginPosPercentage(this->yScaleItem(), percent);
}

void PlotSceneWidget::setDontSavePainterState(bool dont)
{
    setOptimizationFlag(QGraphicsView::DontSavePainterState, dont);
}

void PlotSceneWidget::setDontAdjustForAntialiasing(bool dont)
{
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, dont);
}

void PlotSceneWidget::setCacheBackground(bool cache)
{
    if(cache)
        setCacheMode(QGraphicsView::CacheBackground);
    else
        setCacheMode(QGraphicsView::CacheNone);
}

bool PlotSceneWidget::dontSavePainterState() const
{
    return optimizationFlags() & QGraphicsView::DontSavePainterState;
}

bool PlotSceneWidget::dontAdjustForAntialiasing() const
{
    return optimizationFlags() & QGraphicsView::DontAdjustForAntialiasing;
}

bool PlotSceneWidget::cacheBackground() const
{
    return cacheMode() == QGraphicsView::CacheBackground;
}

/** \brief shortcut to get the axis bound from the xScaleItem()
 *
 * @return xScaleItem->lowerBound
 */
double PlotSceneWidget::xAxisLowerBound() const
{
    return xScaleItem()->lowerBound();
}

/** \brief shortcut to get the axis bound from the yScaleItem()
 *
 * @return yScaleItem->lowerBound
 *
 */
double PlotSceneWidget::yAxisLowerBound() const
{
    return yScaleItem()->lowerBound();
}

/** \brief shortcut to get the axis bound from the xScaleItem()
 *
 * @return xScaleItem->upperBound
 */
double PlotSceneWidget::xAxisUpperBound() const
{
    return xScaleItem()->upperBound();
}

/** \brief shortcut to get the axis bound from the yScaleItem()
 *
 * @return yScaleItem->upperBound
 */
double PlotSceneWidget::yAxisUpperBound() const
{
    return yScaleItem()->upperBound();
}

/** \brief shortcut to get the axis autoscale mode from the xScaleItem()
 *
 * @return xScaleItem()->axisAutoscaleEnabled()
 */
bool PlotSceneWidget::xAxisAutoscaleEnabled() const
{
    return xScaleItem()->axisAutoscaleEnabled();
}

/** \brief shortcut to get the axis autoscale mode from the yScaleItem()
 *
 * @return yScaleItem()->axisAutoscaleEnabled()
 */
bool PlotSceneWidget::yAxisAutoscaleEnabled() const
{
    return yScaleItem()->axisAutoscaleEnabled();
}

/** \brief Shortcut to access the x axis and set the bound.
 *
 */
void PlotSceneWidget::setXAxisLowerBound(double xlb)
{
    xScaleItem()->setLowerBound(xlb);
}

/** \brief Shortcut to access the y axis and set the bound.
 *
 */
void PlotSceneWidget::setYAxisLowerBound(double ylb)
{
    yScaleItem()->setLowerBound(ylb);
}

/** \brief Shortcut to access the x axis and set the bound.
 *
 */
void PlotSceneWidget::setXAxisUpperBound(double xub)
{
    xScaleItem()->setUpperBound(xub);
}

/** \brief Shortcut to access the y axis and set the bound.
 *
 */
void PlotSceneWidget::setYAxisUpperBound(double yub)
{
    yScaleItem()->setUpperBound(yub);
}

/** \brief Shortcut to enable/disable the autoscale on the x axis
 *
 * calls xScaleItem()->setAxisAutoscaleEnabled(en)
 */
void PlotSceneWidget::setXAxisAutoscaleEnabled(bool en)
{
    xScaleItem()->setAxisAutoscaleEnabled(en);
}

/** \brief Shortcut to enable/disable the autoscale on the x axis
 *
 * calls xScaleItem()->setAxisAutoscaleEnabled(en)
 */
void PlotSceneWidget::setYAxisAutoscaleEnabled(bool en)
{
    yScaleItem()->setAxisAutoscaleEnabled(en);
}

/** \brief Shortcut for ScaleItem::axisLabelsOutsideCanvas
 *
 * If set to true, draws the axis labels outside the rectangle where the curves are drawn.
 */
bool PlotSceneWidget::xAxisLabelsOutsideCanvas() const
{
    return xScaleItem()->axisLabelsOutsideCanvas();
}

/** \brief Shortcut for ScaleItem::axisLabelsOutsideCanvas
 *
 * If set to true, draws the axis labels outside the rectangle where the curves are drawn.
 */
bool PlotSceneWidget::yAxisLabelsOutsideCanvas() const
{
    return yScaleItem()->axisLabelsOutsideCanvas();
}

/** \brief Tells the plot to draw the axis labels outside the area
 *         where the curves are drawn.
 *
 * This is useful when the x axis is placed at the bottom.
 */
void PlotSceneWidget::setXAxisLabelsOutsideCanvas(bool outside)
{
    xScaleItem()->setAxisLabelsOutsideCanvas(outside);
}

/** \brief Tells the plot to draw the axis labels outside the area
 *         where the curves are drawn.
 *
 * This is useful when the y axis is placed at the left.
 */
void PlotSceneWidget::setYAxisLabelsOutsideCanvas(bool outside)
{
    yScaleItem()->setAxisLabelsOutsideCanvas(outside);
}

void PlotSceneWidget::setManualSceneUpdate(bool manual)
{
    QTimer *t = findChild<QTimer *>("refreshTimer");
    if(manual)
    {
        setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
        /* if a timer was previously allocated and started, restart it
         * so that the used does not need to call setPeriod again to
         * re activate it.
         */
        if(t)
            t->start();
    }
    else
    {
        /* stop the timer but do not delete it */
        if(t)
            t->stop();
        /* Qt default mode */
        if(!d_ptr->useGl)
            setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        else
            setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    }
}

bool PlotSceneWidget::manualSceneUpdate() const
{
    return viewportUpdateMode() == QGraphicsView::NoViewportUpdate;
}

void PlotSceneWidget::setRefreshPeriod(int period)
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

int PlotSceneWidget::refreshPeriod() const
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
SceneCurve *PlotSceneWidget::addLineCurve(const QString& name, ScaleItem *xScaleI, ScaleItem *yScaleI)
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
    CurveItem *curveItem = new CurveItem(sceneCurve);
    scene()->addItem(curveItem);
    curveItem->setObjectName(name);
    sceneCurve->installCurveChangeListener(curveItem);
    /* painter */
    LinePainter *lp = new LinePainter(curveItem);
    /* automatically pick a color */
    lp->setLineColor(colorPalette.getColor(d_ptr->curveHash.size()));
    lp->setObjectName(name);
    d_ptr->curveHash.insert(name, sceneCurve);
    emit curveAdded(sceneCurve);
    return sceneCurve;
}

void PlotSceneWidget::addCurve(SceneCurve *sceneCurve)
{
    ScaleItem *xScale = sceneCurve->getXAxis();
    ScaleItem *yScale = sceneCurve->getYAxis();
    d_ptr->curveHash.insert(sceneCurve->name(), sceneCurve);
    connect(sceneCurve, SIGNAL(destroyed(QObject*)), this, SLOT(curveAboutToBeDestroyed(QObject*)));
    xScale->installAxisChangeListener(sceneCurve);
    yScale->installAxisChangeListener(sceneCurve);
    emit curveAdded(sceneCurve);
}

SceneCurve *PlotSceneWidget::addCurve(const QString& name)
{
    return addCurve(name, xScaleItem(), yScaleItem());
}

SceneCurve *PlotSceneWidget::addCurve(const QString &name,
                                      ScaleItem* xScaleItem,
                                      ScaleItem* yScaleItem)
{
    if(xScaleItem && yScaleItem)
    {
        SceneCurve *sceneCurve = new SceneCurve(this, name, xScaleItem, yScaleItem);
        d_ptr->curveHash.insert(name, sceneCurve);
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

void PlotSceneWidget::curveAboutToBeDestroyed(const QString& name, bool deleteCurve)
{
    if(d_ptr->curveHash.contains(name))
    {
        SceneCurve *curve = d_ptr->curveHash.value(name);
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
        d_ptr->curveHash.remove(name);
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
void PlotSceneWidget::curveAboutToBeDestroyed(QObject *crv)
{
    SceneCurve *scrv = static_cast<SceneCurve *>(crv);
    if(scrv) /* cleanup, but do not delete (again) the curve */
        this->curveAboutToBeDestroyed(scrv->name(), false);
}

void PlotSceneWidget::removeAxis(QObject *_axis)
{
    ScaleItem* axis = static_cast<ScaleItem *>(_axis);
    this->removePlotGeometryChangeListener(axis);
    d_ptr->zoomer->removeScale(axis);
}

QList<SceneCurve *> PlotSceneWidget::getCurves() const
{
    return d_ptr->curveHash.values();
}

SceneCurve *PlotSceneWidget::findCurve(const QString& name)
{
    if(d_ptr->curveHash.contains(name))
        return d_ptr->curveHash.value(name);
    return NULL;
}

void PlotSceneWidget::appendData(const QString& curveName, double x, double y)
{
    if(d_ptr->curveHash.contains(curveName))
    {
        SceneCurve *c = d_ptr->curveHash.value(curveName);
        c->addPoint(x, y);
    }
    else
        perr("PlotSceneWidget: appendData: no curve with name \"%s\"", qstoc(curveName));
}

void PlotSceneWidget::appendData(const QString& curveName,
                                 const QVector<double>& xData,
                                 const QVector<double> &yData)
{
    if(d_ptr->curveHash.contains(curveName))
    {
        SceneCurve *c = d_ptr->curveHash.value(curveName);
        c->addPoints(xData, yData);
    }
    else
        perr("PlotSceneWidget: appendData (vector version): no curve with name \"%s\"", qstoc(curveName));
}

void PlotSceneWidget::setData(const QString& curveName,
                              const QVector< double > &xData,
                              const QVector< double > &yData)
{
    if(d_ptr->curveHash.contains(curveName))
    {
        SceneCurve *c = d_ptr->curveHash.value(curveName);
        c->setData(xData, yData);
    }
    else
        perr("PlotSceneWidget: setData(): no curve with name \"%s\"", qstoc(curveName));
}

void PlotSceneWidget::setData(const QString& curveName,
                              const QVector< double > &yData)
{
    if(d_ptr->curveHash.contains(curveName))
    {
        SceneCurve *c = d_ptr->curveHash.value(curveName);
        c->setData(yData);
    }
    else
        perr("PlotSceneWidget: setData(yData): no curve with name \"%s\"", qstoc(curveName));
}

ScaleItem *PlotSceneWidget::xScaleItem() const
{
    return d_ptr->axesManager->getAxis(ScaleItem::xBottom);
}

ScaleItem *PlotSceneWidget::yScaleItem() const
{
//    printf("\e[1;31mPlotSceneWidget.yScaleItem: axesManager has axes:\n");
    QList<ScaleItem *> allAxes = d_ptr->axesManager->getAllAxes();
    ScaleItem *y = d_ptr->axesManager->getAxis(ScaleItem::yLeft);
 //   if(!y)
 //       y = d_ptr->axesManager->getAssociatedAxis(ScaleItem::xBottom);
    return y;
}

ScaleItem* PlotSceneWidget::scaleItem(ScaleItem::Id id) const
{
    return d_ptr->axesManager->getAxis(id);
}

ScaleItem* PlotSceneWidget::associatedAxis(ScaleItem::Id otherAxisId) const
{
    return d_ptr->axesManager->getAssociatedAxis(otherAxisId);
}

QPair<double, double> PlotSceneWidget::associatedOriginPosPercentage(ScaleItem::Id xAxisId, ScaleItem::Id yAxisId, bool *ok) const
{
    return d_ptr->axesManager->getOriginPosPercentage(xAxisId, yAxisId, ok);
}

QList<SceneCurve *> PlotSceneWidget::curvesForAxes(ScaleItem::Id axisId, ScaleItem::Orientation orientation) const
{
    QList<SceneCurve *> curves;
    ScaleItem::Id aId;
    foreach(SceneCurve *sc, d_ptr->curveHash.values())
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

void PlotSceneWidget::boundsChanged()
{
    foreach(SceneCurve *sc, d_ptr->curveHash.values())
        sc->invalidateCache();
    /* redraw all the axis */
    foreach(ScaleItem* scaleItem, d_ptr->axesManager->getAllAxes())
        scaleItem->redraw();
}

void PlotSceneWidget::paintEvent(QPaintEvent *event)
{
    if(d_ptr->modifiedPaintEvent)
    {
        QPaintEvent *newEvent=new QPaintEvent(event->region().boundingRect());
        QGraphicsView::paintEvent(newEvent);
        delete newEvent;
    }
    else
        QGraphicsView::paintEvent(event);
}

void PlotSceneWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        d_ptr->mousePressed = true;

    if(event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)
    {
        setDragMode(QGraphicsView::NoDrag);
        d_ptr->mouseMoving = true;
        d_ptr->mousePressedPoint =  event->pos();
    }
    else if(event->button() == Qt::LeftButton && event->modifiers() & Qt::ControlModifier)
        mSwitchAxesCurvesForeground();
    else if(!(event->modifiers() & Qt::ShiftModifier))
        setDragMode(QGraphicsView::ScrollHandDrag);

    foreach(MouseEventListener* l, d_ptr->mouseEventListeners)
        l->mousePressEvent(this, event);
    QGraphicsView::mousePressEvent(event);
}

void PlotSceneWidget::mouseReleaseEvent(QMouseEvent *event)
{
    foreach(MouseEventListener* l, d_ptr->mouseEventListeners)
        l->mouseReleaseEvent(this, event);

    if(d_ptr->mousePressed && event->button() == Qt::LeftButton)
    {
#if QT_VERSION < 0x050000
        foreach(MouseEventListener *l, d_ptr->mouseEventListeners)
            l->mouseClickEvent(this, event->posF());

        emit clicked(event->posF());
#else
        foreach(MouseEventListener *l, d_ptr->mouseEventListeners)
            l->mouseClickEvent(this, event->localPos());

        emit clicked(event->localPos());
#endif
        d_ptr->mousePressed = false;
    }
    if(d_ptr->mouseMoving) /* release after a move with left button pressed */
    {
        d_ptr->mouseMoving = false;
        d_ptr->mouseMovingPoint = event->pos();

        if(d_ptr->mouseZoomEnabled)
        {
            QPointF tmpPoint;
            QPointF mP1 = d_ptr->mousePressedPoint;
            QPointF mP2 = d_ptr->mouseMovingPoint;
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
            d_ptr->zoomer->zoom(selectionRect);
            /* clear the rect drawn during mouse move + left click */
            qobject_cast<GraphicsScene* >(scene())->setZoomRect(QRectF());
        }
        //update(); /* shouldn't be necessary */
    }
    if(event->button() == Qt::MidButton)
    {
        if(d_ptr->mouseZoomEnabled)
        {
            d_ptr->zoomer->unzoom();
        }
    }
    setDragMode(QGraphicsView::NoDrag);
    QGraphicsView::mouseReleaseEvent(event);
}

void PlotSceneWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    foreach(MouseEventListener *l, d_ptr->mouseEventListeners)
        l->mouseDoubleClickEvent(this, event);
    QGraphicsView::mouseDoubleClickEvent(event);
}

void PlotSceneWidget::mSwitchAxesCurvesForeground()
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

void PlotSceneWidget::mouseMoveEvent(QMouseEvent *event)
{
    foreach(MouseEventListener *l, d_ptr->mouseEventListeners)
        l->mouseMoveEvent(this, event);

    if(d_ptr->mouseMoving && d_ptr->mouseZoomEnabled)
    {
        d_ptr->mouseMoving = true;
        ///  d_ptr->mouseMovingPoint = mapToScene(event->pos());
        d_ptr->mouseMovingPoint = event->pos();

        /* let the scene draw the zoom rect with a green dashed line */
        QPointF topLeft, botRight;
        if(d_ptr->mouseMovingPoint.x() > d_ptr->mousePressedPoint.x())
        {
            topLeft.setX(d_ptr->mousePressedPoint.x());
            botRight.setX(d_ptr->mouseMovingPoint.x());
        }
        else
        {
            topLeft.setX(d_ptr->mouseMovingPoint.x());
            botRight.setX(d_ptr->mousePressedPoint.x());
        }
        if(d_ptr->mouseMovingPoint.y() > d_ptr->mousePressedPoint.y())
        {
            topLeft.setY(d_ptr->mousePressedPoint.y());
            botRight.setY(d_ptr->mouseMovingPoint.y());
        }
        else
        {
            topLeft.setY(d_ptr->mouseMovingPoint.y());
            botRight.setY(d_ptr->mousePressedPoint.y());
        }
        QRectF zoomR(topLeft, botRight);
        qobject_cast<GraphicsScene* >(scene())->setZoomRect(this->mapToScene(zoomR.toRect()).boundingRect());
        scene()->update(); /* shouldn't be necessary */
    }
    if(d_ptr->mousePressed)
        d_ptr->mousePressed = false;
    QGraphicsView::mouseMoveEvent(event);
}

void PlotSceneWidget::showEvent(QShowEvent *event)
{
    QGraphicsView::showEvent(event);
    /* only on first show event */
    if(d_ptr->neverShown && d_ptr->sceneRectToWidgetGeometry)
    {
        scene()->setSceneRect(this->viewport()->rect());
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
        boundsChanged();
        d_ptr->neverShown = false;
        notifyPlotAreaChanged();
    }
}

void PlotSceneWidget::fitIn()
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    fitInView(this->sceneRect(), Qt::KeepAspectRatio);

    if(d_ptr->scrollBarsEnabled) /* restore scrollbars, if needed */
    {
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

    notifyPlotAreaChanged();
}

void PlotSceneWidget::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
   // qDebug() << __FUNCTION__ << objectName() << "rezize" << event->oldSize() << event->size()
   //          << "viewportRect" << this->viewport()->rect() << "Spontaneus" << event->spontaneous();

    bool inZoom = false;
    //    bool needNotify = true;
    if(d_ptr->zoomer->inZoom())
        inZoom = true;

    QTransform t = QGraphicsView::transform();
    if(d_ptr->scaleOnResize && !inZoom &&
            ((t.m11() == d_ptr->firstScrollM11 && t.m22() == d_ptr->firstScrollM12)
             || d_ptr->firstScrollM11 < 0 /* never scrolled before */ ))
    {
        /* sceneRectChanged signal will be emitted, so recalculatePlotRect will be invoked
         * and notifyPlotAreaChanged too.
         */
        scene()->setSceneRect(this->viewport()->rect());
    }

    //    qDebug() << !inZoom  << d_ptr->scaleOnResize  <<  !d_ptr->neverShown;
    //    if(!inZoom && d_ptr->scaleOnResize && !d_ptr->neverShown)
    //    {
    //        qDebug() << "resizeEvent (never shown)" << objectName();
    //        QTransform t = QGraphicsView::transform();
    //        if((t.m11() == d_ptr->firstScrollM11 && t.m22() == d_ptr->firstScrollM12)
    //                || d_ptr->firstScrollM11 < 0 /* never scrolled before */ )
    //        {
    //            fitIn();
    //            needNotify = false;
    //        }
    //    }
    //    if(needNotify)
    //        notifyPlotAreaChanged();
}

void PlotSceneWidget::notifyPlotAreaChanged()
{
    QSizeF area = d_ptr->plotRect.size();
    QTransform tr = QGraphicsView::transform();
    area.setWidth(area.width() * tr.m11());
    area.setHeight(area.height() * tr.m22());
//    qDebug() << __FUNCTION__ << objectName() << "plotRect"
//             << d_ptr->plotRect << "area" << area;

    emit plotAreaChanged(area);
    emit viewScaleChanged(tr.m11(), tr.m22());
    emit viewScaleChanged((tr.m11() + tr.m22())/2.0);

    //  printf("\e[1;32m m11: %f m22 %f\e[0m\n", tr.m11(), tr.m22());

    foreach(PlotGeometryEventListener *l, d_ptr->plotGeometryEventListeners)
        l->plotAreaChanged(area);
}

/** \brief notifies PlotChangeListeners that the scrollbar has changed its
 *         value
 *
 * @param value the value of the scrollbar as provided by the horizontalScrollBar
 */
void PlotSceneWidget::hScrollBarValueChanged(int value)
{
    foreach(PlotGeometryEventListener *l, d_ptr->plotGeometryEventListeners)
        l->scrollBarChanged(Qt::Horizontal, value);
}

/** \brief notifies PlotChangeListeners that the scrollbar has changed its
 *         value
 *
 * @param value the value of the scrollbar as provided by the verticalScrollBar
 */
void PlotSceneWidget::vScrollBarValueChanged(int value)
{
    foreach(PlotGeometryEventListener *l, d_ptr->plotGeometryEventListeners)
        l->scrollBarChanged(Qt::Vertical, value);
}

void PlotSceneWidget::sceneRectChanged(const QRectF &)
{
    /* recalculatePlotRect() notifies listeners */
    recalculatePlotRect();
}

bool PlotSceneWidget::event(QEvent *event)
{
    return QGraphicsView::event(event);
}

void PlotSceneWidget::wheelEvent(QWheelEvent *e)
{
    /* propagates the event to scene and items */
    QGraphicsView::wheelEvent(e);
    /* if an item accepts the event, do not process the wheel event */
    if(d_ptr->scaleOnScroll)
    {
        /* With version 2.3.0, zoom is managed changing the axes bounds
         */
        //        if(d_ptr->zoomer && d_ptr->zoomer->stackSize() > 1)
        //        {
        //            QRectF zoomRect = d_ptr->zoomer->unzoom();
        //            if(zoomRect.isValid())
        //            {
        //                fitInView(zoomRect, Qt::KeepAspectRatio);
        //                notifyPlotAreaChanged();
        //            }
        //        }
        //        else
        {
            float dx, dy;
            int d = e->delta();
            /* save the first value of m11 and m22 transform matrix */
            if(d_ptr->firstScrollM11 < 0)
            {
                QTransform t = QGraphicsView::transform();
                d_ptr->firstScrollM11 = t.m11();
                d_ptr->firstScrollM12 = t.m22();
            }
            if(d > 0)
            {
                dx = 1.25;
                dy = 1.25;
            }
            else
            {
                dx = 1.0/1.25;
                dy = 1.0/1.25;
            }
            this->centerOn(e->pos());
            scale(dx, dy);

        }
    }
}

double PlotSceneWidget::transform(const double x, ScaleItem* scaleItem) const
{
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
                len = scaleItem->canvasWidth - 1;
                coord = (x - start) * len / (end - start);
            }
            else
            {
                /* -1 because when x == start the coord must be the canvasHeight - 1
                 * in order to be drawn (coordinates start from 0).
                 * When x == end the coordinate is 0, which is correct.
                 * In practise, the extension must be [0 - scaleItem->canvasHeight - 1].
                 */
                len = scaleItem->canvasHeight - 1;
                coord = len - (x - start) * len / (end - start);
//                qDebug() << __FUNCTION__ << coord << " len " << len << " x " << x << "start" <<
//                            start << "end" << end << " end - start" << end - start;
            }
        }
    }
    else
        perr("PlotSceneWidget::transform: invalid axis (NULL)");
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
QPointF PlotSceneWidget::invTransform(const QPointF& pSceneCoord)
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
double PlotSceneWidget::invTransform(const double pt, ScaleItem* scaleItem) const
{
    if(scaleItem && scaleItem->orientation() == ScaleItem::Horizontal)
        return scaleItem->lowerBound() + pt * (scaleItem->upperBound() - scaleItem->lowerBound()) / scaleItem->canvasWidth;
    else if(scaleItem && scaleItem->orientation() == ScaleItem::Vertical)
        return scaleItem->upperBound() - pt * (scaleItem->upperBound() - scaleItem->lowerBound()) / scaleItem->canvasHeight;

    perr("PlotSceneWidget::invTransform: invalid scale item (NULL)");
    return 0;
}

QPointF PlotSceneWidget::invTransform(const QPointF& pSceneCoord, ScaleItem* xScaleItem, ScaleItem* yScaleItem) const
{
    if(xScaleItem && yScaleItem)
    {
        ScaleItem *xScI = xScaleItem;
        ScaleItem *yScI = yScaleItem;
        qreal px = pSceneCoord.x();
        qreal py = pSceneCoord.y();

        qreal x = xScI->lowerBound() + px * (xScI->upperBound() - xScI->lowerBound()) / xScaleItem->canvasWidth;
        qreal y = yScI->upperBound() - py * (yScI->upperBound() - yScI->lowerBound()) / yScaleItem->canvasHeight;

//        qDebug() << __FUNCTION__ << xScaleItem << yScaleItem << px << py <<xScaleItem->canvasWidth
//                 << yScaleItem->canvasHeight << x << y;

        return QPointF(x, y);
    }
    perr("PlotSceneWidget::invTransform(const QPointF&, ScaleItem*, ScaleItem*): invalid axes");
    return QPointF();
}

QList<SceneCurve*> PlotSceneWidget::getClosest(
        QPointF &closestPos,
        int *closestIndex,
        const QPointF& viewPos)
{
    QPointF scenePos = mapToScene(viewPos.toPoint());
    QList<SceneCurve *> ret;
    closestPos = QPointF();
    SceneCurve *closestCurve = NULL;
    *closestIndex = -1;
    double minDist = 1e9, dist, xp, yp, xc, yc;
    foreach(SceneCurve *c, d_ptr->curveHash.values())
    {
        const QPointF *points = c->points();
        int dataSize = c->data()->size();
        for(int i = 0; i < dataSize; i++)
        {
            xc = points[i].x();
            yc = points[i].y();
            xp = scenePos.x();
            yp = scenePos.y();
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
        foreach(SceneCurve *c, d_ptr->curveHash.values())
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

void PlotSceneWidget::loadConfigurationProperties()
{
    SettingsLoader settingsLoader(d_ptr->settingsKey);
    foreach(QObject *object, d_ptr->configurableObjectsMap.values())
        settingsLoader.loadConfiguration(object);
}

void PlotSceneWidget::setSettingsKey(const QString& key)
{
    d_ptr->settingsKey = key;
}

QString PlotSceneWidget::settingsKey() const
{
    return d_ptr->settingsKey;
}

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
void PlotSceneWidget::addConfigurableObjects(const QString& title, QObject* object)
{
    d_ptr->configurableObjectsMap.insert(title, object);
}

/** \brief removes an object which was previously added with addConfigurableObjects
 *         with the given title.
  *
  * @param title: in the configuration dialog of the plot the object properties
  *        will appear under a tab widget with title as the title page.
  *
  * @see addConfigurableObjects
  */
void PlotSceneWidget::removeConfigurableObjects(const QString& title)
{
    d_ptr->configurableObjectsMap.remove(title);
}

void PlotSceneWidget::resetTransformMatrix()
{
    QTransform t = QGraphicsView::transform();
    t.reset();
    setTransform(t);
    notifyPlotAreaChanged();
}

QMenu * PlotSceneWidget::createContextMenu()
{
    QMenu *menu = new QMenu(this);
    menu->addAction("Configure...", this, SLOT(executePropertyDialog()));
    QAction *fitInViewAction = menu->addAction("Fit in view", this, SLOT(fitIn()));
    fitInViewAction->setToolTip("Fits the plot in the view");
    QAction *resetAction = menu->addAction("Reset Transform", this, SLOT(resetTransformMatrix()));
    resetAction->setToolTip("Reset the transform matrix,\n"
                            "i.e. reset the scale factor to 1:1");
    menu->addSeparator();
    QAction *saveDataAction = menu->addAction("Save Data...", this, SLOT(saveData()));
    saveDataAction->setToolTip("Open a dialog to save data on a file with different format options");
    return menu;
}

void PlotSceneWidget::contextMenuEvent(QContextMenuEvent *)
{
    if(!findChild<PropertyDialog *>())
    {
        QMenu *menu = createContextMenu();
        menu->exec(QCursor::pos());
        delete menu;
    }
}

void PlotSceneWidget::executePropertyDialog()
{
    PropertyDialog *propertyDialog = new PropertyDialog(this);
    propertyDialog->setAttribute(Qt::WA_DeleteOnClose);
    propertyDialog->configureTabs(d_ptr->configurableObjectsMap);
    propertyDialog->setSettingsKey(d_ptr->settingsKey);
    propertyDialog->show();
}

LegendItem *PlotSceneWidget::legendItem() const
{
    return d_ptr->legendItem;
}

QGraphicsZoomer *PlotSceneWidget::zoomer() const
{
    return d_ptr->zoomer;
}

void PlotSceneWidget::setLegendVisible(bool visible)
{
    d_ptr->legendItem->setVisible(visible);
}

bool PlotSceneWidget::legendVisible() const
{
    return d_ptr->legendItem->isVisible();
}

void PlotSceneWidget::setBackgroundColor(const QColor& c) const
{
    scene()->setBackgroundBrush(QBrush(c));
}

QColor PlotSceneWidget::backgroundColor() const
{
    return scene()->backgroundBrush().color();
}

void PlotSceneWidget::saveData()
{
    QList<SceneCurve *> curves = this->getCurves();
    bool timeScale = false;
    if(curves.size())
    {
        ScaleLabelInterface *scaleLabelInterface = xScaleItem()->scaleLabelInterface();
        timeScale = (scaleLabelInterface && scaleLabelInterface->type() == ScaleLabelInterface::TimeScale);
        PlotSceneWidgetSaver saver;
        if(!saver.save(curves, timeScale))
            QMessageBox::critical(this, "Error saving on file", QString("Error saving file \"%1\":\n%2").arg(saver.fileName()).
                                  arg(saver.errorMessage()));
    }
}

