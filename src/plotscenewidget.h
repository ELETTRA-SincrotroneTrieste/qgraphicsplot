#ifndef PLOTSCENEWIDGET_H
#define PLOTSCENEWIDGET_H

#include <QGraphicsView>

class PlotSceneWidgetPrivate;
class ScaleItem;
class SceneCurve;

class PlotSceneWidget : public QGraphicsView
{
    Q_OBJECT

    Q_PROPERTY(bool painterAntiAlias READ painterAntiAlias WRITE setPainterAntiAlias)
    Q_PROPERTY(bool painterHQGLAntiAlias READ painterHQGLAntiAlias WRITE setPainterHQGLAntiAlias)
    Q_PROPERTY(bool smoothPixmapTransform READ smoothPixmapTransform WRITE setSmoothPixmapTransform)
    Q_PROPERTY(bool dontSavePainterState READ dontSavePainterState WRITE setDontSavePainterState)
    Q_PROPERTY(bool dontAdjustForAntialiasing READ dontAdjustForAntialiasing WRITE setDontAdjustForAntialiasing)
    Q_PROPERTY(bool cacheBackground READ cacheBackground WRITE setCacheBackground)
    Q_PROPERTY(bool manualSceneUpdate READ  manualSceneUpdate WRITE setManualSceneUpdate)
//    Q_PROPERTY(bool modifiedPaintEvent READ modifiedPaintEvent WRITE setModifiedPaintEvent)
    Q_PROPERTY(bool scrollBarsEnabled READ scrollBarsEnabled WRITE setScrollBarsEnabled)

public:
    PlotSceneWidget(QWidget *parent, bool useGl = false);
    virtual ~PlotSceneWidget();

    ScaleItem *xScaleItem() const;
    ScaleItem *yScaleItem() const;

    bool painterAntiAlias() const;
    bool smoothPixmapTransform() const;
    bool painterHQGLAntiAlias() const;
    bool dontSavePainterState() const;
    bool dontAdjustForAntialiasing() const;
    bool cacheBackground() const;
    bool manualSceneUpdate() const;
    bool scrollBarsEnabled() const;
    void showEvent(QShowEvent *event);

public slots:
    SceneCurve *addCurve(const QString& name);
    void addCurve(SceneCurve *c);

    void setManualSceneUpdate(bool manual);
    void setDontSavePainterState(bool dont);
    void setDontAdjustForAntialiasing(bool dont);
    void setScrollBarsEnabled(bool en);
    void setPainterHQGLAntiAlias(bool en);
    void setSmoothPixmapTransform(bool enable);
    void setPainterAntiAlias(bool en);
    void setCacheBackground(bool cache);
    void scale(qreal sx, qreal sy);
    void fitIn();
    void setSceneRectToWidgetGeometry(bool en);
    void resetTransformMatrix();

private slots:
    void sceneRectChanged(const QRectF &r);

private:
    PlotSceneWidgetPrivate *d;
};

#endif // PLOTSCENEWIDGET_H
