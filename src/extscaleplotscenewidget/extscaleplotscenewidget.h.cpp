#include "extscaleplotscenewidget.h"
#include "extscaleplotscenewidgetprivate.h"

#include <QGridLayout>
#include "plotscenewidget.h"
#include "axes/scaleitem.h"
#include "curve/scenecurve.h"
#include "externalscalewidget.h"


ExtScalePlotSceneWidget::ExtScalePlotSceneWidget(QWidget *parent) : QWidget(parent)
{
    d_ptr = new ExtScalePlotSceneWidgetPrivate();
    d_ptr->layout = new QGridLayout(this);
    d_ptr->plot = new PlotSceneWidget(this);

    ExternalScaleWidget *xscale = new ExternalScaleWidget(this);
    ExternalScaleWidget *yscale = new ExternalScaleWidget(this);

    d_ptr->plot->xScaleItem()->installAxisChangeListener(xscale);

}

SceneCurve *ExtScalePlotSceneWidget::addCurve(const QString &name)
{
    SceneCurve *c = d_ptr->plot->addCurve(name, xs, ys);
}

