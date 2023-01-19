#include "markeritem.h"
#include "qgraphicsplotitem.h"
#include "markeritemprivate.h"
#include "qgraphicsplotmacros.h"
#include "colors.h"
#include <math.h>
#include <QMouseEvent>
#include <QPainter>
#include <QtDebug>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

MarkerItem::MarkerItem(QGraphicsObject *parent) : QGraphicsObject(parent),
    MouseEventListener()
{
    d_ptr = new MarkerItemPrivate();
    d_ptr->decorationEnabled = true;

    d_ptr->borderColor = KDARKGRAY;
    d_ptr->backgroundColor = KLIGHTGRAY;
    d_ptr->textColor = Qt::black;
    d_ptr->pointBorderColor = KBLUE;
    d_ptr->pointColor = KORANGE;
    d_ptr->radius = 5.0;
    setObjectName("MarkerItem");
    setVisible(false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
}

MarkerItem::~MarkerItem()
{
    qDebug() << "MarkerItem" << __FUNCTION__ << "being destroyed";
}

/** \brief The color of the text. Default is black
 *
 */
QColor MarkerItem::textColor() const
{
    return d_ptr->textColor;
}

/** \brief Returns the color used to fill the point
 *
 * If an invalid color is set (the default), then the circle is drawn but not filled
 *
 * @return the color used to fill in the point.
 *
 * @see setPointColor
 */
QColor  MarkerItem::pointColor() const
{
    return d_ptr->pointColor;
}

/** \brief This property holds the color of the point.
 *
 * Default: blue
 */
QColor MarkerItem::pointBorderColor() const
{
    return d_ptr->pointBorderColor;
}

/** \brief This property determines the background color of the text area
 */
QColor MarkerItem::backgroundColor() const
{
    return d_ptr->backgroundColor;
}

bool MarkerItem::decorationEnabled() const
{
    return d_ptr->decorationEnabled;
}

/** \brief This property determines if the item ignores the scen transformataions.
 *
 * By default, scene transformations are ignored
 */
bool MarkerItem::itemIgnoresTransformations() const
{
    return this->flags() & QGraphicsItem::ItemIgnoresTransformations;
}

void MarkerItem::setItemIgnoresTransformations(bool ign)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations, ign);
}

void MarkerItem::setTextColor(const QColor& c)
{
    d_ptr->textColor = c;
}

/** \brief Set or change the point fill color
 *
 * @param pointC the color to use to fill the circle around the marker point.
 *
 * \par Note
 * Unless setPointColor is called, the MarkerItem only draws the circle, without
 * filling it.
 *
 */
void MarkerItem::setPointColor(const QColor& pointC)
{
    d_ptr->pointColor = pointC;
}

void MarkerItem::setPointBorderColor(const QColor& bc)
{
    d_ptr->pointBorderColor = bc;
}

void MarkerItem::setBackgroundColor(const QColor& bgc)
{
    d_ptr->backgroundColor = bgc;
}

void MarkerItem::setDecorationEnabled(bool en)

{
    d_ptr->decorationEnabled = en;
}

/** \brief This property describes the dimensions of the point drawn by the marker
 *
 */
double MarkerItem::dotRadius() const
{
    return d_ptr->radius;
}

void MarkerItem::setDotRadius(double r)
{
    d_ptr->radius = r;
}

void MarkerItem::removeCurve(SceneCurve *c)
{
    d_ptr->closestCurves.removeAll(c);
}

void MarkerItem::mouseReleaseEvent(QGraphicsPlotItem* plot, QGraphicsSceneMouseEvent* e)
{
    if(e->button() == Qt::MiddleButton) {
        setVisible(false);
    }
}

void MarkerItem::mouseDoubleClickEvent(QGraphicsPlotItem* plot, QGraphicsSceneMouseEvent* e)
{
    if(e->button() == Qt::LeftButton) {
        setVisible(false);
    }
}

void MarkerItem::mouseClickEvent(QGraphicsPlotItem *plot, const QPointF &pos)
{
    d_ptr->plot = plot;
    d_ptr->closestCurves = plot->getClosest(d_ptr->closestPoint, &d_ptr->closestIndex, pos);
    if(d_ptr->closestCurves.isEmpty())
        return;

//    SceneCurve *aClosestCurve = d_ptr->closestCurves.first();
//    QPointF curvePoint = aClosestCurve->points()[d_ptr->closestIndex];
//    setPos(curvePoint);
    setVisible(d_ptr->closestCurves.size());

    /* must reset the bounding rect to the whole visible area.
     * This fixes the condition where you show marker, hide it, zoom
     * and then show again. The bounding rect optimized in paint upon first
     * show may fall outside the new zoomed area.
     */
    QTransform tran = transform();
    QPointF topLeft(0, 0); //  = plot->mapToScene(0, 0);
    QPointF botRight = plot->mapFromParent(plot->boundingRect().bottom(), plot->boundingRect().right());
    double x = topLeft.x();
    double y = topLeft.y();
    x = tran.m11() * x + tran.m21() * y + tran.dx();
    y = tran.m22() * y + tran.m12() * x + tran.dy();
    QPointF newTopLeft(x, y);
    x = botRight.x();
    y = botRight.y();
    x = tran.m11() * x + tran.m21() * y + tran.dx();
    y = tran.m22() * y + tran.m12() * x + tran.dy();
    QPointF newBotRight(x, y);
    /* reset bounding rect so that the update that follows works even in the case
     * described in the comment above
     */
    d_ptr->boundingRect = QRectF(newTopLeft, newBotRight);
//    qDebug() << __FUNCTION__ << d_ptr->boundingRect;

//    plot->scene()->update();
    setZValue(plot->getCurves().size() + 1);
    update();
}

void MarkerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w)
{
   // painter->setClipRect(option->exposedRect.toRect());
    QFont f = painter->font();
    f.setBold(true);
    painter->save();
    painter->setFont(f);
    QFontMetrics fm(f);
    int idx = 0;
    int txtW, txtH = fm.height() + 2;
    double x, xl;
    d_ptr->boundingRect = QRectF();
    QString txt, curveName;
    if(!d_ptr->closestPoint.isNull() && d_ptr->closestCurves.size())
    {
        QColor txtBgColor = Qt::white;
        txtBgColor.setAlpha(200);
        QRect txtRSum;
        SceneCurve *aClosestCurve = d_ptr->closestCurves.first();
        QPointF curvePoint = aClosestCurve->points()[d_ptr->closestIndex];
        QPen p(d_ptr->pointBorderColor);
    //    QBrush b(d_ptr->pointColor);
        painter->setPen(p);
    //    painter->setBrush(b);
        double dy = curvePoint.y();
        double dx = curvePoint.x();

        QTransform tran = transform();
        dx = tran.m11() * dx + tran.m21() * dy + tran.dx();
        dy = tran.m22() * dy + tran.m12() * dx + tran.dy();

//        qDebug() << "Transform " << sceneTransform() << " this " << this->transform()
//                 << view->transform();

        QRect dotRect(dx - d_ptr->radius/2.0, dy - d_ptr->radius/2.0, d_ptr->radius, d_ptr->radius);
        painter->drawEllipse(dotRect);

       // double w = aClosestCurve->scene()->sceneRect().width();
       // double h = aClosestCurve->scene()->sceneRect().height();
        double x = 0, y = 0;
        QPointF topLeft(5,5); // = view->mapToScene(5, 5);
        x = topLeft.x();
        y = topLeft.y();
        x = tran.m11() * x + tran.m21() * y + tran.dx();
        y = tran.m22() * y + tran.m12() * x + tran.dy();
        foreach(SceneCurve *c, d_ptr->closestCurves)
        {
            if(d_ptr->closestIndex < c->dataSize())
            {
                QRect txtR;
                QString sx, sy;
                /* obtain the label which may differ from the data value if a ScaleLabelInterface
                 * implementation was installed
                 */
                sx = c->getXAxis()->label(c->data()->xData.at(d_ptr->closestIndex));
                sy = c->getYAxis()->label(c->data()->yData.at(d_ptr->closestIndex));
                curveName = c->property("alias").toString();
                if(curveName.isEmpty())
                    curveName = c->name();
                curveName = curveName.section('/', curveName.count('/') -1, curveName.count('/'));
                txt = QString("%1 {%2} [%3]").arg(curveName).arg(sx).arg(sy);
                txtW = fm.horizontalAdvance(txt);

//                if(d_ptr->closestPoint.x() > w / 2)
//                    x = dx - txtW - d_ptr->radius;
//                else
//                    x = dx + d_ptr->radius;

//                if(d_ptr->closestPoint.y() > h / 2)
//                    y = dy - txtH * (idx + 1);
//                else
//                    y = dy + d_ptr->radius;
                y += txtH * idx;
                txtR = QRect(QPoint(x, y), QSize(txtW, txtH * (idx+ 1) ));

                txtRSum = txtRSum.united(txtR);
                painter->setPen(KDARKGRAY);
//                qDebug() << txtR << txt;
           //     painter->drawRect(txtR);
                painter->fillRect(txtR, (txtBgColor));
                painter->setPen(QPen(d_ptr->textColor));
                painter->drawText(txtR, txt);

                /* draw line */
                x = txtR.left() + txtW / 2;
                if(dx < x)
                    xl = 1;
                else
                    xl = -1;
                painter->setPen(d_ptr->pointBorderColor);
                painter->drawLine(x, txtR.bottom(),
                                  dx + xl * d_ptr->radius * sqrt(2)/2, dy - d_ptr->radius / sqrt(2)/2);
                idx++;
            }
        }
        d_ptr->boundingRect =  (txtRSum.united(dotRect));
    }

   painter->drawRect(d_ptr->boundingRect);

    painter->restore();
}

/* optimized in paint, reset in mouse press */
QRectF MarkerItem::boundingRect() const
{
   return d_ptr->boundingRect;
}
