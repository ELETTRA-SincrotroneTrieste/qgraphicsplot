#ifndef MARKERITEM_H
#define MARKERITEM_H

#include <QGraphicsObject>
#include <mouseeventlistener.h>
#include <scenecurve.h>

class MarkerItemPrivate;
class SceneCurve;

/** \brief When a mouse click event takes place into the plot, the closest point
 *         on the closest curve is marked by a round circle and the x and y values
 *         of the point on the curve are drawn.
 *
 * The marker is drawn on the point of the curve closest to the point
 * where the mouse click event took place.
 *
 * \par Implementation
 * When the user clicks on a point P1 on the plot, the MarkerItem asks the plot which are the
 * curve and the point closest to P1. A circle is drawn on that point and its position is kept
 * up to date when the <em>y</em> coordinate changes in time. The point is kept drawn on the
 * same <em>x </em> coordinate.
 *
 * \note Important note
 * If your application <em>permits removing curves</em>, then you <em>must connect the
 * PlotSceneWidget::curveAboutToBeRemoved signal to the MarkerItem::removeCurve slot</em>.
 * <em>Forgetting to do this will cause the application to crash</em> when a curve on which
 * a marker is placed is removed. <br/>
 * It is not otherwise possible for the MarkerItem to know  when a curve is removed, because
 * the closest x coordinate point and the closest curve are only calculated on the click event and are not
 * updated at each paint event, for performance reasons.
 * You don't need this signal/slot connection if you don't plan to remove curves from the plot
 * at runtime.
 *
 * MarkerItem is configurable through some Qt properties. If you want to be able to change them
 * at runtime, remember to call PlotSceneWidget::addConfigurableObjects after creating your own
 * MarkerItem.
 *
 * \par Example
 * \code
 *   MarkerItem* marker = new MarkerItem(0);
 *   myPlotSceneWidget->installMouseEventListener(marker);
 *   myPlotSceneWidget->scene()->addItem(marker);
 *   // if you plan to remove a curve from the plot
 *   connect(myPlotSceneWidget, SIGNAL(curveAboutToBeRemoved(SceneCurve *)), marker,
 *           SLOT(removeCurve(SceneCurve *)));
 *   // if you want to configure the MarkerItem at runtime:
 *   myPlotSceneWidget->addConfigurableObjects("Marker", marker);
 * \endcode
 *
 * If the <em>alias</em> Qt property is set on a scene curve, this is used by the marker
 * to draw the curve identifier instead of its name (SceneCurve::name).
 *
 */
class MarkerItem : public QGraphicsObject, public MouseEventListener
{
    Q_OBJECT

    Q_PROPERTY(bool decorationEnabled READ decorationEnabled WRITE setDecorationEnabled)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
    Q_PROPERTY(QColor pointColor READ pointColor WRITE setPointColor)
    Q_PROPERTY(QColor pointBorderColor READ pointBorderColor WRITE setPointBorderColor)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(double dotRadius READ dotRadius WRITE setDotRadius)
    Q_PROPERTY(bool itemIgnoresTransformations READ itemIgnoresTransformations WRITE setItemIgnoresTransformations)

public:

    enum { Type = UserType + 201 };

    MarkerItem(QGraphicsObject *parent);

    virtual ~MarkerItem();

    virtual void mouseClickEvent(QGraphicsPlotItem *plot, const QPointF &pos);

    virtual void mouseReleaseEvent(QGraphicsPlotItem* plot, QGraphicsSceneMouseEvent* e);

    virtual void mouseDoubleClickEvent(QGraphicsPlotItem *, QGraphicsSceneMouseEvent *);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QRectF boundingRect() const;

    QColor textColor() const;

    QColor  pointColor() const;

    QColor pointBorderColor() const;

    QColor backgroundColor() const;

    bool decorationEnabled() const;

    double dotRadius() const;

    bool itemIgnoresTransformations() const;

public slots:
    void setTextColor(const QColor& c);

    void setPointColor(const QColor& pointC);

    void setPointBorderColor(const QColor& bc);

    void setBackgroundColor(const QColor& bgc);

    void setDecorationEnabled(bool en);

    void setDotRadius(double r);

    void removeCurve(SceneCurve *c);

    void setItemIgnoresTransformations(bool ign);

private:
    MarkerItemPrivate *d_ptr;

    Q_DECLARE_PRIVATE(MarkerItem)
};

#endif // MARKERITEM_H
