#include "targetitem.h"
#include "targetitemprivate.h"
#include "../colors.h"
#include "qgraphicsplotitem.h"
#include "itemmovelistener.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtDebug>

/** \brief The TargetItem constructor
  *
  * @param xAxis a reference to the associated x ScaleItem
  * @param yAxis a reference to the associated x ScaleItem
  * @param plot the PlotSceneWidget in which to place the TargetItem
  *
  * Initiates a red TargetItem with an inner radius of 15 and an outer radius of 20.
  * The item is movable, selectable, sends geometry changes and sends scene position
  * changes (QGraphicsItem's item flags).
  *
  * The position with respect to the x and y scales is set to 0.0 by default.
  * Remember to call setX and setY when you know what the position of the item
  * is. Storing x and y position in axes coordinates is important in order to reposition
  * the item when the canvas rect of the plot changes.
  *
  * \par Automatic repositioning of the TargetItem on plot geometry change event.
  * When the PlotSceneWidget plot canvas changes its geometry, the canvasRectChanged method
  * repositions the target item according to the new geometry and the values of x and y
  * axes coordinates of the target items, which are set with setX and setY
  * The TargetItem constructor installs the TargetItem on each axis, xAxis and yAxis,
  * in order to automatically manage the object repositioning when the canvas plot rect
  * changes.
  */
TargetItem::TargetItem(ScaleItem* xAxis, ScaleItem* yAxis, QGraphicsPlotItem* plot) :
    QGraphicsObject(NULL)
{
    d_ptr = new TargetItemPrivate();
    d_ptr->radius = 40.0;
    d_ptr->circleColor = KRED;
    d_ptr->circleColor.setAlpha(120);
    d_ptr->itemMoveListener = NULL;
    d_ptr->mousePressed = false;
    d_ptr->pointLabelEnabled = true;
    d_ptr->innerRadius = 15.0;
    d_ptr->outerRadius = 20.0;
    d_ptr->radiusScaleDivider = 1.0;
    d_ptr->plot = plot;
    d_ptr->xAxis = xAxis;
    d_ptr->yAxis = yAxis;
    d_ptr->x = 0.0;
    d_ptr->y = 0.0;
    setFlags(QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemSendsGeometryChanges|
             QGraphicsItem::ItemSendsScenePositionChanges);

    Q_ASSERT(xAxis != NULL && yAxis != NULL && plot != NULL);

    d_ptr->xAxis->installAxisChangeListener(this);
    d_ptr->yAxis->installAxisChangeListener(this);
    printf("\e[1;31mTargetItem.TargetItem: new item %p as axis change lis %p\e[0m\n", this, static_cast<AxisChangeListener *>(this));
}

TargetItem::~TargetItem() {

}

/** \brief Installs an ItemMoveListener implementation
  *
  * @param l an object implementing the ItemMoveListener interface.
  * @param NULL if you want to remove a previously installed listener.
  *
  * @see ItemMoveListener
  * @see itemMoveListener
  *
  */
void TargetItem::installItemMoveListener(ItemMoveListener *l)
{
    d_ptr->itemMoveListener = l;
}

/** \brief Returns the currently installed item move listener
 *
 * @return the currently installed item move listener, NULL if none is installed
 */
ItemMoveListener *TargetItem::itemMoveListener() const
{
    return d_ptr->itemMoveListener;
}

void TargetItem::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem * option,
                       QWidget *)
{
    double innerRadius = d_ptr->innerRadius / d_ptr->radiusScaleDivider;
    double outerRadius = d_ptr->outerRadius / d_ptr->radiusScaleDivider;
    painter->setClipRect(option->exposedRect);
    painter->setPen(QPen(d_ptr->circleColor.darker()));
    if(d_ptr->mousePressed)
    {
        QColor c = KDARKGREEN;
        painter->setPen(c);
        painter->setBrush(d_ptr->circleColor.lighter());
    }
    else
    {
        painter->setPen(QPen(d_ptr->circleColor.darker()));
        painter->setBrush(d_ptr->circleColor);
    }
    painter->drawEllipse(QPointF(0, 0), outerRadius, outerRadius);

    QColor brushColor(Qt::white);
    if(d_ptr->mousePressed)
    {
        brushColor.setAlpha(60);
        painter->setPen(KDARKGREEN);
    }
    else
    {
        painter->setPen(Qt::black);
        brushColor.setAlpha(120);
    }
    painter->setBrush(brushColor);
    painter->drawEllipse(QPointF(0, 0), innerRadius, innerRadius);

    painter->setPen(Qt::black);
    painter->drawLine(0, -innerRadius, 0, innerRadius);
    painter->drawLine(-innerRadius, 0, innerRadius, 0);

    if(d_ptr->pointLabelEnabled && d_ptr->mousePressed)
    {
        double x = d_ptr->plot->invTransform(pos().x(), d_ptr->xAxis);
        double y = d_ptr->plot->invTransform(pos().y(), d_ptr->yAxis);
        QFont f = painter->font();
        double pointSize = 14 /  d_ptr->radiusScaleDivider;
        if(pointSize < 5)
            pointSize = 5;
        f.setPointSizeF(pointSize);
        painter->setFont(f);
        painter->setPen(KRED);
        QString txt = QString("(%1,%2)").arg(x).arg(y);
        painter->drawText(QPointF(0, 0), txt);
        QFontMetrics fm(f);

        /* for bounding rect */
        d_ptr->textHeight = fm.height();
        d_ptr->textWidth = fm.width(txt);
    }
    else
    {
        d_ptr->textHeight = 0;
        d_ptr->textWidth = 0;
    }
}

/** \brief This method updates the item position when the plot canvas changes.
  *
  *
  */
void  TargetItem::plotAreaChanged(const QRectF &newRect)
{
    Q_UNUSED(newRect)
    updatePosition();
}

/** \brief updates the item position according to the x and y values in scale coordinates
  *
  * @see setX
  * @see setY
  * @see x
  * @see y
  *
  */
void TargetItem::updatePosition()
{
    setPos(d_ptr->plot->transform(d_ptr->x, d_ptr->xAxis),
                              d_ptr->plot->transform(d_ptr->y, d_ptr->yAxis));
}

/** \brief returns the x position in the ScaleItem x coordinate system
  *
  * @return the x position in the ScaleItem x coordinate system
  *
  * Must have previously been set with setX
  *
  * @see setX
  * @see y
  *
  */
double TargetItem::x() const
{
    return d_ptr->x;
}

/** \brief returns the y position in the ScaleItem y coordinate system
  *
  * @return the y position in the ScaleItem y coordinate system
  *
  * Must have previously been set with setY
  *
  * @see setY
  * @see x
  *
  */
double TargetItem::y() const
{
    return d_ptr->y;
}

/** \brief stores the x position in the x scale coordinate.
  *
  * @param x the x value, in the ScaleItem coordinates (x axis coordinate)
  *
  * Storing the x value in the target item allows its position to be automatically
  * updated when the canvas rect of the plot changes.
  *
  * @see setY
  * @see y
  * @see x
  */
void TargetItem::setX(double x)
{
    d_ptr->x = x;
}

/** \brief stores the y position in the y scale coordinate.
  *
  * @param y the y value, in the ScaleItem coordinates (y axis coordinate)
  *
  * Storing the y value in the target item allows its position to be automatically
  * updated when the canvas rect of the plot changes.
  *
  * @see setX
  * @see y
  * @see x
  */
void TargetItem::setY(double y)
{
    d_ptr->y = y;
}

QVariant TargetItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene() && d_ptr->mousePressed)
    {
        // value is the new position.
        QPointF newPos = value.toPointF();
        if(d_ptr->itemMoveListener)
            d_ptr->itemMoveListener->itemMoved(newPos);
    }
    return QGraphicsItem::itemChange(change, value);
}

QRectF TargetItem::boundingRect () const
{
    double outerRadius = d_ptr->outerRadius / d_ptr->radiusScaleDivider;
    QRectF br = QRectF(-outerRadius, -outerRadius, 2* outerRadius, 2 * outerRadius);
    /* text is drawn above the circle */
    if(br.width() < d_ptr->textWidth)
        br.setWidth(d_ptr->textWidth * 1.5);
    return br;
}

void TargetItem::setCircleColor(const QColor &c)
{
    d_ptr->circleColor = c;
    update();
}

void TargetItem::setPointLabelEnabled(bool en)
{
    d_ptr->pointLabelEnabled = en;
    update();
}

void TargetItem::setInnerRadius(double r)
{
    d_ptr->innerRadius = r;
    update();
}

void TargetItem::setOuterRadius(double r)
{
    d_ptr->outerRadius = r;
    update();
}

void TargetItem::setRadiusScaleDivider(double divider)
{
    d_ptr->radiusScaleDivider = divider;
    update();
}

double TargetItem::radiusScaleDivider() const
{
    return d_ptr->radiusScaleDivider;
}

QColor TargetItem::circleColor() const
{
    return d_ptr->circleColor;
}

bool TargetItem::pointLabelEnabled() const
{
    return d_ptr->pointLabelEnabled;
}

double TargetItem::innerRadius() const
{
    return d_ptr->innerRadius;
}

double TargetItem::outerRadius() const
{
    return d_ptr->outerRadius;
}

void TargetItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    d_ptr->mousePressed = true;
    QGraphicsObject::mousePressEvent(event);
    update();
}

void TargetItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    d_ptr->mousePressed = false;
    QGraphicsObject::mouseReleaseEvent(event);
    update();
}

void TargetItem::xAxisBoundsChanged(double , double )
{
    updatePosition();
}

void TargetItem::yAxisBoundsChanged(double , double )
{
    updatePosition();
}

