#include "graphicsscene.h"
#include "graphicsscene_private.h"
#include "colors.h"
#include <QPainter>
#include <QtDebug>
#include <QGraphicsView>

GraphicsScene::GraphicsScene(QObject * parent ) : QGraphicsScene(parent)
{
    d_ptr = new GraphicsScenePrivate();
}

void GraphicsScene::drawForeground( QPainter * painter, const QRectF & )
{
    if(d_ptr->zoomRect.isValid())
    {
        QPen p = painter->pen();
        p.setColor(KDARKGREEN);
        p.setStyle(Qt::DashLine);
        painter->setPen(p);
        painter->drawRect(d_ptr->zoomRect);
    }
}

QRectF GraphicsScene::zoomRect() const
{
    return d_ptr->zoomRect;
}

void GraphicsScene::setZoomRect(const QRectF& p)
{
    d_ptr->zoomRect = p;
}

