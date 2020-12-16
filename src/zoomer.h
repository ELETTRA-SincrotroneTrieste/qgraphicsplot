#ifndef ZOOMER_H
#define ZOOMER_H

class PlotSceneWidget;

class ZoomerPrivate;
class QRectF;

class Zoomer
{
public:
    explicit Zoomer(PlotSceneWidget *plot);

    void zoom(const QRectF& zoomRect);

    void setZoomRect(const QRectF& r);

    void setBaseScale(double sx, double sy);

    bool inZoom() const;

    void unzoom();


private:
    ZoomerPrivate *d_ptr;
};

#endif // ZOOMER_H
