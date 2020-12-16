#ifndef EXTSCALEPLOTSCENEWIDGET_H
#define EXTSCALEPLOTSCENEWIDGET_H

class ExtScalePlotSceneWidgetPrivate;
class SceneCurve;
class PlotSceneWidget;
class ScaleItem;

#include <scaleitem.h>
#include <QWidget>

class ExtScalePlotSceneWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExtScalePlotSceneWidget(QWidget *parent = 0, bool multiScale = false);

    SceneCurve * addCurve(const QString& name, ScaleItem *xScale = NULL, ScaleItem *yScale = NULL,
                          bool addExtScale = false);

    ScaleItem* addAxis(ScaleItem::Orientation o, const QString& axisName);

    void addExternalScale(ScaleItem *axis, SceneCurve *curve);

    PlotSceneWidget *getPlot() const;

    void setMultiscale(bool multiscale);

    bool multiscale() const;

    ScaleItem *getAxis(ScaleItem::Id);

    void removeCurve(const QString& name);

    void clear();

    SceneCurve *addCurve(SceneCurve *curve, bool addExtScale);

    void removeAdditionalExternalScales();

signals:

public slots:

private:

    ScaleItem *configureNewScale(ScaleItem::Orientation, const QString &curveName);

    ExtScalePlotSceneWidgetPrivate *d_ptr;
};

#endif // EXTSCALEPLOTSCENEWIDGET_H
