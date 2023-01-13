#ifndef LEGENDITEM_H
#define LEGENDITEM_H

#include <QGraphicsObject>
#include <QGraphicsScene>

class SceneCurve;
class LegendItemPrivate;

/** \brief A legend item.
 *
 * This class is created and managed by the PlotSceneWidget.
 *
 * \par Configuration
 * The legend can be configured by changing its zValue (default value is 100),
 * by filling its background and drawing a black bounding rect around (both
 * disabled by default.
 *
 * \par Accessing the LegendItem.
 * Use PlotSceneWidget::legendItem()
 *
 * \par Curve aliases.
 * The legend is able to show an alias instead of the complete source name of the curve.
 * Use setCurveAlias to setup an alias for each curve. setCurveAlias can be invoked
 * before or after TPlotSceneWidget::setSourcesList
 */
class LegendItem : public QGraphicsObject
{
    Q_PROPERTY(bool isVisible READ isVisible WRITE setVisible)
    Q_PROPERTY(double zValue READ zValue WRITE setZValue)
    Q_PROPERTY(bool fillBackground READ fillBackground WRITE setFillBackground)
    Q_PROPERTY(bool drawBoundingRect READ drawBoundingRect WRITE setDrawBoundingRect)

    Q_OBJECT
public:
    explicit LegendItem(QGraphicsItem *parent);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool fillBackground() const;

    bool drawBoundingRect() const;

    bool isVisible() const;

    QRectF boundingRect() const;

    void setCurveAlias(const QString& realName, const QString& alias);

    QString curveAlias(const QString& realName) const;

    QString curveRealName(const QString& alias) const;

    void removeCurveAlias(const QString& realName);
    
    QColor getColor(SceneCurve *curve);
signals:
    
public slots:

    void setVisible(bool isVisible);

    void newCurveAdded(SceneCurve *curve);

    void curveRemoved(SceneCurve *c);

    void setDrawBoundingRect(bool br);

    void setFillBackground(bool fill);


private:
    LegendItemPrivate *d_ptr;
    
};

#endif // LEGENDITEM_H
