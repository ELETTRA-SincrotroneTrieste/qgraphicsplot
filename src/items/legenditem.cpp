#include "legenditem.h"
#include "scenecurve.h"
#include "curveitem.h"
#include "../curve/painters/linepainter.h"
#include "../curve/painters/dotspainter.h"
#include "../curve/painters/histogrampainter.h"
#include "../curve/painters/stepspainter.h"

#include <QList>
#include <QFontMetrics>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QFont>
#include <QtDebug>
#include <QHash>
#include <qgraphicsplotmacros.h>

class LegendItemPrivate
{
public:
    double width, height;

    bool fillBackground, drawBoundingRect;

    void updateLongestCurveName();

    void addCurve(SceneCurve *crv);

    void removeCurve(const QString& name);

    QString longestCurveName;

    /* curve name, SceneCurve */
    QHash<QString, SceneCurve *> curves;

    /* curve name, curve alias */
    QHash<QString, QString> curveAliases;

};

void LegendItemPrivate::addCurve(SceneCurve *crv)
{
    curves.insert(crv->name(), crv);
    updateLongestCurveName();
}

void LegendItemPrivate::removeCurve(const QString& name)
{
    curves.remove(name);
    curveAliases.remove(name);
}

void LegendItemPrivate::updateLongestCurveName()
{
    QString cName;
    foreach(SceneCurve *c, curves)
    {
        if(curveAliases.contains(c->name()))
            cName = curveAliases[c->name()];
        else
            cName = c->name();

        if(cName.length() > longestCurveName.length())
            longestCurveName = cName;
    }
}

LegendItem::LegendItem(QGraphicsScene *) :
    QGraphicsObject(0)
{
    d_ptr = new LegendItemPrivate();
    d_ptr->width = 300;
    d_ptr->height = 400;
    d_ptr->drawBoundingRect = d_ptr->fillBackground = false;
    setZValue(100);
}

void LegendItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    int visibleCurvesCount = 0;
    const double margin = 5.0;
    const double hmargin = 2;
    double y = pos().y();
    QFont f = painter->font();
    QFontMetrics fm(f);
    d_ptr->width = fm.width(d_ptr->longestCurveName) + margin;
    d_ptr->height = 0;
    foreach(SceneCurve *sc, d_ptr->curves)
        if(sc->curveItem() && sc->curveItem()->isVisible())
            visibleCurvesCount++;
    d_ptr->height = visibleCurvesCount * (fm.height() + hmargin) + hmargin;

    QString text;
    /* clip rect */
    painter->setClipRect(option->rect);

    if(d_ptr->fillBackground)
    {
        QColor c = Qt::white;
        painter->fillRect(0, 0, d_ptr->width, d_ptr->height, QBrush(c));
    }
    if(d_ptr->drawBoundingRect)
    {
        painter->setPen(Qt::black);
        painter->drawRect(0, 0, d_ptr->width, d_ptr->height);
    }

    foreach(SceneCurve * sc, d_ptr->curves)
    {
        if(sc->curveItem() && sc->curveItem()->isVisible())
        {
            painter->setPen(getColor(sc));
            if(d_ptr->curveAliases.contains(sc->name()))
                text = d_ptr->curveAliases.value(sc->name());
            else
                text = sc->name();
            painter->drawText(QRectF(0, y, d_ptr->width, fm.height()), Qt::AlignRight|Qt::AlignVCenter, text);
            y += hmargin + fm.height();

        }
    }

    double xPos = scene()->sceneRect().width() - d_ptr->width;
    if(xPos != pos().x())
    {
        setPos(scene()->sceneRect().width() - d_ptr->width, pos().y());
    }

    //    if(pos().x() != scene()->sceneRect().width() - d_ptr->width)
    //    {
    //        qDebug() << "LegendItem::paint(): adjusting position" << pos();
    //        moveBy(scene()->sceneRect().width() - d_ptr->width, 0);
    //    }
}

QRectF LegendItem::boundingRect() const
{
    return QRectF(0, 0, d_ptr->width + 1, d_ptr->height + 1);
}

QColor LegendItem::getColor(SceneCurve *curve)
{
    QColor color = QColor();
    CurveItem *curveItem = curve->curveItem();
    if(curveItem)
    {
        ItemPainterInterface *ipi = curve->curveItem()->itemPainter();
        if(ipi)
        {
            color = ipi->pen().color();
        }
    }
    if(!color.isValid())
        perr("LegendItem::getColor: could not get color for curve \"%s\": are curveItem and ItemPainter set?",
             qstoc(curve->name()));
    return color;
}

void LegendItem::newCurveAdded(SceneCurve *curve)
{
    d_ptr->addCurve(curve);
}

void LegendItem::curveRemoved(SceneCurve *c)
{
    d_ptr->removeCurve(c->name());
    d_ptr->updateLongestCurveName();
    update();
}

bool LegendItem::fillBackground() const
{
    return d_ptr->fillBackground;
}

bool LegendItem::drawBoundingRect() const
{
    return d_ptr->drawBoundingRect;
}

void LegendItem::setDrawBoundingRect(bool br)
{
    d_ptr->drawBoundingRect = br;
    update();
}

void LegendItem::setFillBackground(bool fill)
{
    d_ptr->fillBackground = fill;
    update();
}

void LegendItem::setVisible(bool visible)
{
    QGraphicsObject::setVisible(visible);
}

bool LegendItem::isVisible() const
{
    return QGraphicsObject::isVisible();
}

void LegendItem::setCurveAlias(const QString& realName, const QString& alias)
{
    d_ptr->curveAliases.insert(realName, alias);
    d_ptr->updateLongestCurveName();
    update();
}

QString LegendItem::curveAlias(const QString& realName) const
{
    return d_ptr->curveAliases.value(realName);
}

QString LegendItem::curveRealName(const QString& alias) const
{
    return d_ptr->curveAliases.key(alias);
}

void LegendItem::removeCurveAlias(const QString& realName)
{
    d_ptr->curveAliases.remove(realName);
}



