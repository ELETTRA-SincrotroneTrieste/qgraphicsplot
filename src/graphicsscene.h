#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QRectF>

class GraphicsScenePrivate;


class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GraphicsScene(QObject * parent = 0);

    void setZoomRect(const QRectF& p);

    QRectF zoomRect() const;

public slots:

protected:
    void drawForeground(QPainter * painter, const QRectF & rect );

private:
    Q_DECLARE_PRIVATE(GraphicsScene)
    GraphicsScenePrivate *d_ptr;
};

#endif // GRAPHICSSCENE_H
