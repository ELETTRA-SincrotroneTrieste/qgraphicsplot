#include "zoomer.h"
#include <plotscenewidget.h>
#include <QList>
#include <QPair>
#include <QRectF>
#include "zoomer.h"
#include <QtDebug>

class ZoomerPrivate
{
public:
    QRectF zoomRect;
    PlotSceneWidget* plot;
    bool inZoom;

    QList<QPair <double, double> > zoomStack;
};


Zoomer::Zoomer(PlotSceneWidget *plot)
{
    d_ptr = new ZoomerPrivate();
    d_ptr->plot = plot;
    d_ptr->inZoom = false;
    d_ptr->zoomRect = plot->plotRect();
    d_ptr->zoomStack.append(QPair <double, double> (1.0, 1.0));
}

void Zoomer::unzoom()
{
    qDebug() << "zoom out" << d_ptr->zoomRect;
    if(d_ptr->inZoom && d_ptr->zoomStack.size())
    {
        QPair <double, double> lastScale = d_ptr->zoomStack.last();
        d_ptr->zoomStack.removeLast();
        d_ptr->plot->scale(lastScale.first, lastScale.second);
    }
    d_ptr->inZoom = false;
}

void Zoomer::zoom(const QRectF& zoomRect)
{
    double sx, sy, w, h;
    double x = zoomRect.x(), y = zoomRect.y();
    if(zoomRect.x() < d_ptr->plot->plotRect().left())
        x =d_ptr->plot->plotRect().left();
    if(zoomRect.x() > d_ptr->plot->plotRect().right())
        x =d_ptr->plot->plotRect().right();

    if(zoomRect.y() < d_ptr->plot->plotRect().top())
        y =d_ptr->plot->plotRect().top();
    if(zoomRect.y() > d_ptr->plot->plotRect().bottom())
        y =d_ptr->plot->plotRect().bottom();

    qDebug() << "zoom in" << zoomRect;

    w = d_ptr->plot->plotRect().width();
    h = d_ptr->plot->plotRect().height();

    sx = x * d_ptr->zoomStack.last().first / w;
    sy = y * d_ptr->zoomStack.last().second / h;

    qDebug() << "fattori scala sx sy" << sx << sy << " 1/sx, 1/sy " << 1.0/sx << 1.0/sy;
    d_ptr->inZoom = true;

    d_ptr->plot->scale(1.0/sx, 1.0/sy);
    d_ptr->zoomStack.append(QPair<double, double> (sx, sy));
}

void Zoomer::setZoomRect(const QRectF &r)
{
    d_ptr->zoomRect = r;
}

bool Zoomer::inZoom() const
{
    return d_ptr->inZoom;
}

void Zoomer::setBaseScale(double sx, double sy)
{
    QPair <double, double> basePair(sx, sy);
    basePair.first = sx;
    basePair.second = sy;
    d_ptr->zoomStack[0] = basePair;
}
