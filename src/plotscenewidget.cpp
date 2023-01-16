#include "plotscenewidget.h"
#include "qgraphicsplotitem.h"
#include "scaleitem.h"
#include <QApplication>
#include <QGLWidget>
#include <graphicsscene.h>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>
#include <QtDebug>

class PlotSceneWidgetPrivate {
public:
    bool useGl, scrollBarsEnabled, sceneRectToWidgetGeometry;
    QGraphicsPlotItem *ploti;
};

PlotSceneWidget::PlotSceneWidget(QWidget *parent, bool useGl) : QGraphicsView(parent)
{
    d = new PlotSceneWidgetPrivate;

    if(useGl || qApp->arguments().contains("--use-gl"))
    {
        d->useGl = true;
        QGLWidget *glWidget = new QGLWidget(QGLFormat(QGL::SampleBuffers));
        glWidget->makeCurrent();
        this->setViewport(glWidget);
        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        printf("\e[1;32m* \e[0musing openGL\n");
    }

    qDebug() << __PRETTY_FUNCTION__ << "viewport update mode is " << viewportUpdateMode();

    /* set a scene rect because ScaleItem needs a fixed scene rect.
     * The scene rect will be updated on showEvent
     */
    GraphicsScene *scene = new GraphicsScene(this);
    scene->setSceneRect(0, 0, 500, 400);
    setScene(scene);
    setMouseTracking(true);
    setRenderHint(QPainter::Antialiasing);
    setCursor(QCursor(Qt::CrossCursor));
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHints( QPainter::SmoothPixmapTransform);

    d->scrollBarsEnabled = true;
    d->ploti = new QGraphicsPlotItem(nullptr);
    d->ploti->setOriginPosPercentage(d->ploti->xScaleItem(), 0.0);
    d->ploti->setOriginPosPercentage(d->ploti->yScaleItem(), 0.0);
    d->ploti->setYAxisLabelsOutsideCanvas(true);
    scene->addItem(d->ploti);

    connect(scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));


    /* when the user scrolls the view, immediately update the scene without waiting for the
     * scene updates deriving from curve updates or from other timer updates.
     */
    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), scene, SLOT(update()));
    connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)), scene, SLOT(update()));

    /* this is to notify PlotEventListener classes that the scroll bar values have changed.
     * For instance, an ExternalScaleWidget must be informed when a scroll bar value changes.
     */
    connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(hScrollBarValueChanged(int)));

    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(vScrollBarValueChanged(int)));
    scene->update();
}

PlotSceneWidget::~PlotSceneWidget()
{
    delete d;
}

ScaleItem *PlotSceneWidget::xScaleItem() const {
    return d->ploti->xScaleItem();
}

ScaleItem *PlotSceneWidget::yScaleItem() const {
    return d->ploti->yScaleItem();
}

QGraphicsPlotItem *PlotSceneWidget::plot() const {
    return d->ploti;
}

void PlotSceneWidget::sceneRectChanged(const QRectF& r)
{
    scene()->update();
}

void PlotSceneWidget::wheelEvent(QWheelEvent *e)
{
        /* propagates the event to scene and items */
        QGraphicsView::wheelEvent(e);
        /* if an item accepts the event, do not process the wheel event */
//        if(d_ptr->scaleOnScroll)
        {
            /* With version 2.3.0, zoom is managed changing the axes bounds
             */
            //        if(d_ptr->zoomer && d_ptr->zoomer->stackSize() > 1)
            //        {
            //            QRectF zoomRect = d_ptr->zoomer->unzoom();
            //            if(zoomRect.isValid())
            //            {
            //                fitInView(zoomRect, Qt::KeepAspectRatio);
            //                notifyPlotAreaChanged();
            //            }
            //        }
            //        else
            {
                float dx, dy;
                int d = e->delta();
                /* save the first value of m11 and m22 transform matrix */
//                if(d_ptr->firstScrollM11 < 0)
//                {
//                    QTransform t = QGraphicsView::transform();
//                    d_ptr->firstScrollM11 = t.m11();
//                    d_ptr->firstScrollM12 = t.m22();
//                }
                if(d > 0)
                {
                    dx = 1.25;
                    dy = 1.25;
                }
                else
                {
                    dx = 1.0/1.25;
                    dy = 1.0/1.25;
                }
                this->centerOn(e->pos());
                scale(dx, dy);
            }
        }
}


void PlotSceneWidget::setPainterAntiAlias(bool en)
{
    setRenderHint(QPainter::Antialiasing, en);
}

bool PlotSceneWidget::painterAntiAlias() const
{
    return this->renderHints() & QPainter::Antialiasing;
}

void PlotSceneWidget::setPainterHQGLAntiAlias(bool en)
{
    setRenderHint(QPainter::HighQualityAntialiasing, en);
}

void PlotSceneWidget::setSmoothPixmapTransform(bool enable)
{
    setRenderHint(QPainter::SmoothPixmapTransform, enable);
}

bool PlotSceneWidget::smoothPixmapTransform() const
{
    return renderHints() & QPainter::SmoothPixmapTransform;
}

void PlotSceneWidget::setScrollBarsEnabled(bool en)
{
    d->scrollBarsEnabled = en;
    if(!en)
    {
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else
    {
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}

bool PlotSceneWidget::painterHQGLAntiAlias() const
{
    return this->renderHints() & QPainter::HighQualityAntialiasing;
}

void PlotSceneWidget::scale(qreal sx, qreal sy)
{
    QGraphicsView::scale(sx, sy);
    //    qDebug() << "scale: sceneRect" << this->sceneRect() << "scene->sceneRect" <<
    //                scene()->sceneRect();
    //    qDebug() << QGraphicsView::transform();
    //    notifyPlotAreaChanged();
}

void PlotSceneWidget::fitIn()
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    fitInView(this->sceneRect(), Qt::KeepAspectRatio);

    if(d->scrollBarsEnabled) /* restore scrollbars, if needed */
    {
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

    //    notifyPlotAreaChanged();
}

void PlotSceneWidget::setDontSavePainterState(bool dont)
{
    setOptimizationFlag(QGraphicsView::DontSavePainterState, dont);
}

void PlotSceneWidget::setDontAdjustForAntialiasing(bool dont)
{
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, dont);
}

void PlotSceneWidget::setCacheBackground(bool cache)
{
    if(cache)
        setCacheMode(QGraphicsView::CacheBackground);
    else
        setCacheMode(QGraphicsView::CacheNone);
}

bool PlotSceneWidget::dontSavePainterState() const
{
    return optimizationFlags() & QGraphicsView::DontSavePainterState;
}

bool PlotSceneWidget::dontAdjustForAntialiasing() const
{
    return optimizationFlags() & QGraphicsView::DontAdjustForAntialiasing;
}

bool PlotSceneWidget::cacheBackground() const
{
    return cacheMode() == QGraphicsView::CacheBackground;
}

bool PlotSceneWidget::manualSceneUpdate() const
{
    return viewportUpdateMode() == QGraphicsView::NoViewportUpdate;
}

bool PlotSceneWidget::scrollBarsEnabled() const {
    return d->scrollBarsEnabled;
}

void PlotSceneWidget::setSceneRectToWidgetGeometry(bool en)
{
    d->sceneRectToWidgetGeometry = en;
}

void PlotSceneWidget::resetTransformMatrix()
{
    QTransform t = QGraphicsView::transform();
    t.reset();
    setTransform(t);
    //    notifyPlotAreaChanged();
}

void PlotSceneWidget::showEvent(QShowEvent *event)
{
    /* only on first show event */
    //    if(d_ptr->neverShown && d_ptr->sceneRectToWidgetGeometry)
    //    {
    //        scene()->setSceneRect(this->viewport()->rect());
    //        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    //        boundsChanged();
    //        d_ptr->neverShown = false;
    //        notifyPlotAreaChanged();
    //    }

}

SceneCurve * PlotSceneWidget::addCurve(const QString &name) {
   return d->ploti->addCurve(name);
}

void PlotSceneWidget::addCurve(SceneCurve *c) {
    d->ploti->addCurve(c);
}

void PlotSceneWidget::setManualSceneUpdate(bool manual)
{
    setViewportUpdateMode(manual ? QGraphicsView::NoViewportUpdate : QGraphicsView::FullViewportUpdate);
}
