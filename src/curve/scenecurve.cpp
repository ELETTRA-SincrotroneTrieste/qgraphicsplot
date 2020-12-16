#include "scenecurve.h"
#include "scenecurveprivate.h"
#include "pointdata.h"
#include "scaleitem.h"
#include "point.h"
#include "curvechangelistener.h"
#include "qgraphicsplotmacros.h"
#include "pointprivate.h"
#include "curveitem.h"
#include <math.h> /* for isnan() */
#include <QtDebug>
#include <QPainterPath>


/** \brief Constructor of a SceneCurve object
  *
  * Constructs a new SceneCurve, which collects a number of XYItems internally.
  *
  * @param sceneWidget the PlotSceneWidget to which the curve must belong
  * @param name a compulsory name representing the curve.
  * @param xAxis a reference to an existing ScaleItem to use as y axis
  * @param yAxis a reference to an existing ScaleItem to use as y axis
  *
  * \par Concepts.
  * Each curve belongs to a PlotSceneWidget and has two axes associated.
  * The SceneCurve implements the AxisChangeListener interface in order to know
  * when the axes bounds change and the axis autoScale property changes.
  *
  * \par Curve drawing.
  * The SceneCurve does not draw anything on the scene. Its just recollects
  * data and keeps a list of points whose coordinates map the real points into
  * the plot coordinate system. In order to draw, you must create a CurveItem
  * and then attach it to the SceneCurve, normally constructing a CurveItem with
  * the SceneCurve as parameter.
  *
  * @see CurveItem::CurveItem
  *
  * \par Example. Adding a curve to a PlotSceneWidget
  * \code
  * SceneCurve *curve = mPlot->addCurve("Curve Name");
  * // if you want no more than bufsize points on the curve
  * int bufsize = 1000;
  * curve->setBufferSize(bufsize);
  *
  * // Create the effective curve painter!
  * // Just pass the curve in the constructor
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
  *
  * \par Item painters ownership
  * The CurveItem takes the ownership of every installed ItemPainterInterface implementation
  * instance. This means that all the installed ItemPainterInterface objects are deleted on
  * CurveItem deletion.
  *
  * \par CurveItem ownership
  * If you call PlotSceneWidget::removeCurve, then the associated CurveItem is deleted.
  * If you delete a SceneCurve without invoking PlotSceneWidget::removeCurve (bad), then
  * the associated CurveItem is deleted again for you.
  * This means that once installed on a SceneCurve, the ownership of the CurveItem is
  * taken by the SceneCurve. With the term <em>install</em> we mean the
  *
  * \code
  * CurveItem *curveItem = new CurveItem(curve);
  * \endcode
  * instruction.
  *
  * \par Obtaining the CurveItem QGraphicsObject objects associated to the SceneCurve
  * An internal reference to the (unique) CurveItem object is kept in order to be able to provide it to
  * the users of SceneCurve.
  * The method
  * curveItem() can be used to retrieve a reference to the unique CurveItem attached to
  * the curve. For instance to change the colour of the line at any time, having a reference
  * to a SceneCurve:
  *
  * \code
  * LinePainter *currentLinePainter = curve->curveItem()->linePainter();
  * currentLinePainter->setLineColor(Qt::blue);
  * \endcode
  *
  * \note The code above only works if a unique LinePainter was installed, just like in the
  * <em>Example</em> in the previous paragraph. If a LinePainter coexists with a DotsPainter,
  * for instance, you need to carefully read the note below.
  *
  * \note Multiple ItemPainterInterface implementation instances can be installed on a CurveItem.
  *       If they all are QObject-derived, then a Qt property can be used to distinguish between
  *       different painters. For instance:
  * \code
  * linePainter->setProperty("type", "Lines");
  * \endcode
  * Moreover, the ItemPainterInterface::type virtual method, which is implemented by all
  * ItemPainterInterface derived classes provides a key to distinguish among predefined
  * and user defined painters. The ItemPainterInterface can be statically casted to the
  * correct class type.
  * @see ItemPainterInterface::type()
  *
  * \par Retrieving an installed LinePainter, given a SceneCurve
  * \code
  * CurveItem *curveItem = sceneCurve->curveItem();
  * // get all the installed item painters
  * QList<ItemPainterInterface *>itemPainters = curveItem->itemPainters();
  * // and look for the LinePainter
  * foreach(ItemPainterInterface *ipi, itemPainters)
  * {
  *   if(ipi->type() == ItemPainterInterface::Line)
  *     static_cast<LinePainter *>(ipi)->setLineColor(Qt::darkRed);
  * }
  *\endcode
  *
  * \par The alias property.
  * The Qt <em>alias</em> property can be set so that some objects can use it to display
  * the curve name instead of the string returned by SceneCurve::name method.
  *
  * \par Note: NaN values
  * The NaN y values are mapped on the same y as the previous y value. If the first point of
  * a curve is NaN, then the lower bound is chosen for the NaN representation.
  *
  * <strong>Important</strong>: it is up to the <strong>painter</strong> to evidentiate a NaN
  * value. This must be done inspecting the y data value of a point.
  *
  * @see PlotSceneWidget
  * @see Data
  * @see CurveItem
  * @see LinePainter
  * @see Dotspainter
  * @see HistogramPainter
  * @see ItemPainterInterface
  */
SceneCurve::SceneCurve(PlotSceneWidget *sceneWidget,
                       const QString &name,
                       ScaleItem *xAxis,
                       ScaleItem *yAxis) :
    QObject(sceneWidget),
    AxisChangeListener()
{
    Q_ASSERT(xAxis != NULL);
    Q_ASSERT(yAxis != NULL);
    Q_ASSERT(sceneWidget != NULL);

    d_ptr = new SceneCurvePrivate();
    d_ptr->plot = sceneWidget;
    d_ptr->name = name;
    d_ptr->xAxis = xAxis;
    d_ptr->yAxis = yAxis;
    d_ptr->lastValidXPos = d_ptr->lastValidYPos = -1;
    d_ptr->curveItem = NULL;
    /* by default buffer size is unlimited */
    d_ptr->bufferSize = -1;

    //   this->installCurveChangeListener(xAxis);
    //   this->installCurveChangeListener(yAxis);
    /* initialize the object name to be able to use Q_PROPERTY with PropertyDialog
     */
    setObjectName(name);

    d_ptr->data = new Data();

    /* curves may be added after canvas rect change notification has arrived
     */
    canvasRectChanged(d_ptr->plot->plotRect());
}

SceneCurve::~SceneCurve()
{
    qDebug() << __FUNCTION__ << "deleting scene curve " << objectName();
}

QString SceneCurve::name() const
{
    return d_ptr->name;
}

PlotSceneWidget* SceneCurve::plot() const
{
    return d_ptr->plot;
}

int SceneCurve::dataSize() const
{
    return d_ptr->data->size();
}

QGraphicsScene *SceneCurve::scene() const
{
    return d_ptr->plot->scene();
}

void SceneCurve::setBufferSize(int size)
{
    if(size > 0)
    {
        d_ptr->bufferSize = size;
        foreach(CurveChangeListener *listener, d_ptr->itemChangeListeners)
            listener->bufferSizeChanged(size);
    }
}

int SceneCurve::bufferSize() const
{
    return d_ptr->bufferSize;
}

void SceneCurve::removeCurveChangeListener(CurveChangeListener *listener)
{
    d_ptr->itemChangeListeners.removeAll(listener);
}

void SceneCurve::installCurveChangeListener(CurveChangeListener *listener)
{
    d_ptr->itemChangeListeners.append(listener);
}

/** \brief Removes a CurveItem from the list of items associated to the curve
 *
 * When a CurveItem is destroyed, this method must be called in order to tell
 * the SceneCurve that a CurveItem has been removed.
 * This method removes the CurveItem from the internal list of CurveItem objects
 *
 * @see CurveItem::~CurveItem
 */
void SceneCurve::removeCurveItem()
{
    d_ptr->curveItem = NULL;
}

/** \brief This method adds to the list of CurveItem objects an already allocated
 *         CurveItem.
 *
 * An internal list of CurveItem objects is kept in order to be able to provide to
 * the users of SceneCurve a list of associated CurveItems.
 */
void SceneCurve::setCurveItem(CurveItem *curveItem)
{
    d_ptr->curveItem = curveItem;
}

/** \brief Returns the last curve item that was attached to the curve.
 *
 * \note This method can be used when you are sure that only one CurveItem
 * has been associated to the curve, because it returns only the last element
 * in the list of the potentially multiple CurveItem objects associated to the
 * SceneCurve.
 *
 * @see curveItems
 */
CurveItem* SceneCurve::curveItem() const
{
    return d_ptr->curveItem;
}

void SceneCurve::setXDataIsOrdered(bool ordered)
{
    d_ptr->data->xDataOrdered = ordered;
}

void SceneCurve::setYDataIsOrdered(bool ordered)
{
    d_ptr->data->yDataOrdered = ordered;
}

bool SceneCurve::xDataIsOrdered() const
{
    return d_ptr->data->xDataOrdered;
}

bool SceneCurve::yDataIsOrdered() const
{
    return d_ptr->data->yDataOrdered;
}

QList<CurveChangeListener *> SceneCurve::curveChangeListeners() const
{
    return d_ptr->itemChangeListeners;
}

ScaleItem::Id SceneCurve::associatedXAxisId() const
{
    return d_ptr->xAxis->axisId();
}

ScaleItem::Id SceneCurve::associatedYAxisId() const
{
    return d_ptr->yAxis->axisId();
}

void SceneCurve::invalidateCache()
{
    d_ptr->lastValidXPos = -1;
    d_ptr->lastValidYPos = -1;
    d_ptr->mPoints.clear();
}

void SceneCurve::invalidateXCache()
{
    d_ptr->lastValidXPos = -1;
    d_ptr->mPoints.clear();
}

void SceneCurve::invalidateYCache()
{
    d_ptr->lastValidYPos = -1;
    d_ptr->mPoints.clear();
}

ScaleItem* SceneCurve::getXAxis() const
{
    return d_ptr->xAxis;
}

ScaleItem* SceneCurve::getYAxis() const
{
    return d_ptr->yAxis;
}
double  SceneCurve::getMinX() const
{
    return d_ptr->data->xMin;
}

double  SceneCurve::getMinY() const
{
    return d_ptr->data->yMin;
}

double  SceneCurve::getMaxX() const
{
    return d_ptr->data->xMax;
}

double  SceneCurve::getMaxY() const
{
    return d_ptr->data->yMax;
}

void SceneCurve::canvasRectChanged(const QRectF& r)
{
    d_ptr->canvasRectTop = r.top();
    d_ptr->canvasRectW = r.width();
    d_ptr->canvasRectH = r.height();
    d_ptr->canvasRectLeft = r.left();
    invalidateCache();
}

void SceneCurve::xAxisBoundsChanged(double xl, double xu)
{
    d_ptr->xlb = xl;
    d_ptr->xub = xu;
    d_ptr->xextension = xu - xl;
    invalidateXCache();
}
void SceneCurve::yAxisBoundsChanged(double yl, double yu)
{
    d_ptr->ylb = yl;
    d_ptr->yub = yu;
    d_ptr->yextension = yu - yl;
    invalidateYCache();
}

void SceneCurve::axisAutoscaleChanged(ScaleItem::Orientation o, bool autoscale)
{
    if(autoscale)
    {
        switch(o)
        {
        case ScaleItem::Horizontal:
            d_ptr->data->calculateXBounds();
            break;
        case ScaleItem::Vertical:
            d_ptr->data->calculateYBounds();
            break;
        }
    }
}

Data *SceneCurve::data() const
{
    return d_ptr->data;
}

/** \brief Add a couple of points (x,y) to the curve
 *
 */
void SceneCurve::addPoint(double x, double y)
{
    /* remove items if the size is about to be greater than bufferSize */
    mCheckBufferSize();

    /* addPoint updates max and min of the curve */
    d_ptr->data->addPoint(x, y);
    d_ptr->data->scalarMode = true;

    foreach(CurveChangeListener *listener, d_ptr->itemChangeListeners)
    {
        listener->itemAdded(Point(x, y));
    }
}

void SceneCurve::setData(const QVector<double>& xData, const QVector<double> &yData)
{
    d_ptr->data->setData(xData, yData);
    d_ptr->data->scalarMode = false;


    if(d_ptr->xAxis->axisAutoscaleEnabled() && d_ptr->yAxis->axisAutoscaleEnabled())
        d_ptr->data->calculateBounds(); /* just one cycle */
    else if(d_ptr->xAxis->axisAutoscaleEnabled())
        d_ptr->data->calculateXBounds();
    else if(d_ptr->yAxis->axisAutoscaleEnabled())
        d_ptr->data->calculateYBounds();

    if(!d_ptr->plot->manualSceneUpdate())
    {
        foreach(CurveChangeListener *listener, d_ptr->itemChangeListeners)
            listener->fullVectorUpdate();
    }
}


/** \brief This convenience method <em>appends</em> a vector of data to a scalar curve
 *
 * \note scalar data.
 */
void SceneCurve::addPoints(const QVector<double>& xData, const QVector<double> &yData)
{
    if(xData.size() == 1 && yData.size() == 1)
        addPoint(xData.first(), yData.first());
    else
    {
        d_ptr->data->addPoints(xData, yData);
        d_ptr->data->scalarMode = true;

        /* the same is equivalent to setData above */
        if(d_ptr->xAxis->axisAutoscaleEnabled() && d_ptr->yAxis->axisAutoscaleEnabled())
            d_ptr->data->calculateBounds(); /* just one cycle */
        else if(d_ptr->xAxis->axisAutoscaleEnabled())
            d_ptr->data->calculateXBounds();
        else if(d_ptr->yAxis->axisAutoscaleEnabled())
            d_ptr->data->calculateYBounds();

        if(!d_ptr->plot->manualSceneUpdate())
        {
            foreach(CurveChangeListener *listener, d_ptr->itemChangeListeners)
                listener->fullVectorUpdate();
        }

    }
}

void SceneCurve::setData(const QVector<double> &yData)
{
    d_ptr->data->yData = yData;

    if(d_ptr->curveItem && d_ptr->curveItem->isVisible())
    {
        foreach(CurveChangeListener *listener, d_ptr->itemChangeListeners)
            listener->fullVectorUpdate();
    }
}

const QPointF *SceneCurve::points()
{
    int siz = d_ptr->data->size();

    if(siz <= 0)
        return NULL;

    if(d_ptr->data->dataUnchanged() && d_ptr->mPoints.size())
    {
        return d_ptr->mPoints.constData();
    }


    int index;
    /* calls of points() between subsequend calls of setData/appendData do not need
         * to recalculate all the points. Data is not changed.
         */
    d_ptr->data->cacheData();

    if(siz != d_ptr->mPoints.size())
        d_ptr->mPoints.resize(siz);

    for(index = 0; index < siz; index++)
    {
        double xp = 0.0;
        if(index < d_ptr->data->size())
        {
            //        printf("--- must recalc X pos cuz idx %d lastValidX %d\n", index, d_ptr->lastValidXPos);
            double x = d_ptr->data->xData[index];
            if(d_ptr->xub == d_ptr->xlb)
                return NULL;
            xp = (d_ptr->canvasRectW - 1) * (x - d_ptr->xlb) / (d_ptr->xextension) + d_ptr->canvasRectLeft;

        }
        else
            printf("Data::xpos: index %d outside bounds (%d)\n", index, d_ptr->data->size());

        double yp = 0.0;
        if(index < d_ptr->data->size())
        {
            //   printf("--- must recalc Y pos cuz idx %d lastValidX %d\n", index, d_ptr->lastValidXPos);
            double y = d_ptr->data->yData[index];
            if(d_ptr->yub == d_ptr->ylb)
                return NULL;

            /* if y is NaN, then let the curve display the first valid value on the y axis */
            if(isnan(y)) /* put the previous value if available, lower bound otherwise */
            {
                int backidx = index;
                while(--backidx >= 0)
                {
                    if(!isnan(d_ptr->data->yData[backidx]))
                    {
                        y = d_ptr->data->yData[backidx];
                        break;
                    }
                }
                if(backidx < 0)
                    y = d_ptr->ylb;
            }

            yp = d_ptr->canvasRectH - 1 - ((d_ptr->canvasRectH - 1) * (y - d_ptr->ylb) / (d_ptr->yextension) + d_ptr->canvasRectTop);
        }
        else
            printf("Data::ypos: index %d outside bounds (%d)\n", index, d_ptr->data->size());


        d_ptr->mPoints[index] = QPointF(xp, yp);
    }
    /* we made the data object calculate all the positions for its points.
         * From the curve point of view, all its points positions are determined
         * We mark the x and y scene coordinates positions valid.
         */
    d_ptr->lastValidXPos = index - 1;
    d_ptr->lastValidYPos = index - 1;

    //   qDebug() << "returning point as constData" << d_ptr->mPoints.constData() << "size" << d_ptr->mPoints.size()
    //            << "dataSize " << d_ptr->data->size();
    return d_ptr->mPoints.constData();
}


bool SceneCurve::mRemovedItemAffectsBounds(const Point& toRemovePt)
{
    return (toRemovePt.x == d_ptr->data->xMin) || (toRemovePt.x == d_ptr->data->xMax) ||
            (toRemovePt.y == d_ptr->data->yMin) || (toRemovePt.y == d_ptr->data->yMax);
}

int SceneCurve::mCheckBufferSize()
{
    QList<Point> itemsRemoved;
    int itemCount = d_ptr->data->size();
    bool removedItemAffectsBounds = false;
    if(d_ptr->bufferSize > -1)
    {
        /* +1 because this is called before adding a new item
         */
        while(itemCount + 1 - d_ptr->bufferSize > 0)
        {
            Point firstPoint = d_ptr->data->point(0);

            /* notify to the listener if installed */
            foreach(CurveChangeListener *listener, d_ptr->itemChangeListeners)
                listener->itemAboutToBeRemoved(firstPoint);

            /* remove x and y data associated to the index of the first element */
            d_ptr->data->remove(0);

            itemsRemoved << firstPoint;

            itemCount--;

            /* test whether the removal of the item may affect the x and y max
             * and min values of the curve.
             * Check only if the boolean removedItemAffectsBounds is still false.
             * If set to true, at the end of the cycle a full recalculation is
             * needed for max and min.
             */
            if(!removedItemAffectsBounds)
            {
                removedItemAffectsBounds = mRemovedItemAffectsBounds(firstPoint);
            }
        }
        /* update curve y and x minimum and maximum values */
        if(removedItemAffectsBounds)
        {
            d_ptr->data->calculateBounds();
        }
        /* at the end, notify which items have been removed. At this point,
         * the curve bounds are up to date, so that the itemRemoved listeners can
         * obtain the correct bounds of the curve
         */
        foreach(Point point, itemsRemoved)
        {
            foreach(CurveChangeListener *listener, d_ptr->itemChangeListeners)
                listener->itemRemoved(point);
        }

        if(removedItemAffectsBounds)
        {
            /* each listener must obtain again max and min from curves */
            foreach(CurveChangeListener *listener, d_ptr->itemChangeListeners)
                listener->affectingBoundsPointsRemoved();
        }
    }
    return itemCount;
}
