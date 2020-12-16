#include "extscaleplotscenewidget.h"
#include "extscaleplotscenewidgetprivate.h"

#include <QtDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include "plotscenewidget.h"
#include "axes/scaleitem.h"
#include "curve/scenecurve.h"
#include "externalscalewidget.h"
#include "qgraphicsplotmacros.h"
#include "qgraphicszoomer.h"
#include "curveitem.h"
#include "painters/linepainter.h"
#include "axiscouple.h"
#include "axesmanager.h"
#include "scalelabels/timescalelabel.h"
#include "../../src/colors.h"

ExtScalePlotSceneWidget::ExtScalePlotSceneWidget(QWidget *parent, bool multiScale) : QWidget(parent)
{
    bool useOpenGl = false;
    d_ptr = new ExtScalePlotSceneWidgetPrivate();
    d_ptr->layout = new QGridLayout(this);
    d_ptr->yLeftLo = new QHBoxLayout(this);
    d_ptr->yRightLo = new QHBoxLayout(this);
    d_ptr->plot = new PlotSceneWidget(this, false, useOpenGl);
    d_ptr->yLeftLo->setObjectName("yLeftLayout");
    d_ptr->yRightLo->setObjectName("yRightLayout");
    d_ptr->layout->setObjectName("plotLayout");
    d_ptr->multiscale = multiScale;

    /* add x axis */
    d_ptr->plot->addAxis(ScaleItem::Horizontal, ScaleItem::xBottom, NULL);

    ExternalScaleWidget *xscale = new ExternalScaleWidget(this, ScaleItem::Horizontal);
    d_ptr->layout->addWidget(d_ptr->plot, 0, 1, 10, 10);
    d_ptr->layout->addLayout(d_ptr->yLeftLo, 0, 0, 10, 1);
    d_ptr->layout->addLayout(d_ptr->yRightLo, 0, 11, 10, 1);
    d_ptr->layout->addWidget(xscale, 10, 1, 1, 10);

    d_ptr->plot->scaleItem(ScaleItem::xBottom)->installAxisChangeListener(xscale);
    d_ptr->plot->scaleItem(ScaleItem::xBottom)->setAxisLabelsEnabled(false);
    d_ptr->plot->installPlotGeometryChangeListener(xscale);

    connect(d_ptr->plot->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            xscale, SLOT(scrollChanged(int)));
    xscale->setAlignment(Qt::AlignHCenter);
    xscale->setObjectName("extXDefaultScale");
    xscale->setTickStepLen(d_ptr->plot->xScaleItem()->tickStepLen());
    xscale->setLabelRotation(60);
    TimeScaleLabel *timeScaleLabel = new TimeScaleLabel(true);
    xscale->installScaleLabelInterface(timeScaleLabel);
    xscale->setOriginPercentage(d_ptr->plot->originPosPercentage(d_ptr->plot->xScaleItem()));
    xscale->setMargin(d_ptr->plot->frameWidth());
}

/** \brief Add a pre configured curve to the plot
 *
 * @param curve a previously configured curve to be added to the plot
 *
 */
SceneCurve *ExtScalePlotSceneWidget::addCurve(SceneCurve *curve, bool addExtScale)
{
    if(addExtScale)
        addExternalScale(curve->getYAxis(), curve);
    d_ptr->curvesMap.addCurve(curve->name(), curve);
    d_ptr->plot->addCurve(curve);
    return curve;
}

SceneCurve *ExtScalePlotSceneWidget::addCurve(const QString &name,
                                              ScaleItem *xScale,
                                              ScaleItem *yScale,
                                              bool addExtScale)
{
    QList<QColor> palette = QList<QColor> () << KDARKWATER << KDARKBLUE << KGRAY <<
                                                KYELLOW << KCAMEL << KDARKCYAN <<
                                                KDARKPINK << KVERYDARKVIOLET;
    SceneCurve *c = d_ptr->curvesMap.getCurve(name);
    if(!c)
    {
        if(xScale == NULL)
            xScale = d_ptr->plot->xScaleItem();
        if(yScale == NULL)
            yScale = d_ptr->plot->yScaleItem();

        c = new SceneCurve(d_ptr->plot, name, xScale, yScale);
        qDebug() << __FUNCTION__ << d_ptr->curvesMap.size() << "added " << name << " scale Item" << yScale;
        /* curves represented by lines */
        CurveItem *curveItem = new CurveItem(c);
        d_ptr->plot->scene()->addItem(curveItem);
        c->installCurveChangeListener(curveItem);
        LinePainter *lp = new LinePainter(curveItem);
        lp->setLineColor(palette.at(d_ptr->curvesMap.size() % palette.size()));
        if(addExtScale)
            addExternalScale(yScale, c);

        d_ptr->curvesMap.addCurve(name, c);
        d_ptr->plot->addCurve(c);
        return c;
    }
    else
    {
        perr("ExtScalePlotSceneWidget::addCurve: curve %s already present", qstoc(name));
    }
    return c;
}

ScaleItem *ExtScalePlotSceneWidget::addAxis(ScaleItem::Orientation o, const QString &axisName)
{
    return configureNewScale(o, axisName);
}

void ExtScalePlotSceneWidget::addExternalScale(ScaleItem *axis, SceneCurve *curve)
{
    qDebug() << __FUNCTION__ << axis->objectName() << curve->name() << "curves im map " << d_ptr->curvesMap.size();
    ExternalScaleWidget *yscale = new ExternalScaleWidget(this, ScaleItem::Vertical);
    QHBoxLayout *lo;
    if(d_ptr->plot->axesManager()->count() % 2)
    {
        lo = d_ptr->yRightLo;
        yscale->setTickDrawingInverted(true);
    }
    else
        lo = d_ptr->yLeftLo;

    lo->insertWidget(0, yscale);

    axis->installAxisChangeListener(yscale);
    d_ptr->plot->installPlotGeometryChangeListener(yscale);

    connect(d_ptr->plot->verticalScrollBar(), SIGNAL(valueChanged(int)),
            yscale, SLOT(scrollChanged(int)));
    yscale->setAlignment(Qt::AlignVCenter);
    yscale->setTickStepLen(axis->tickStepLen());
    yscale->setOriginPercentage(d_ptr->plot->originPosPercentage(axis));
    yscale->setMargin(d_ptr->plot->frameWidth());
    yscale->setPen(curve->curveItem()->itemPainter()->pen());
    yscale->setToolTip(curve->name());
    yscale->setObjectName(curve->name()+ " Y axis");
}

PlotSceneWidget *ExtScalePlotSceneWidget::getPlot() const
{
    return d_ptr->plot;
}

void ExtScalePlotSceneWidget::setMultiscale(bool multiscale)
{
    d_ptr->multiscale = multiscale;
}

bool ExtScalePlotSceneWidget::multiscale() const
{
    return d_ptr->multiscale;
}

ScaleItem *ExtScalePlotSceneWidget::getAxis(ScaleItem::Id id)
{
    return d_ptr->plot->axesManager()->getAxis(id);
}

void ExtScalePlotSceneWidget::removeCurve(const QString &name)
{
    if(d_ptr->plot->findCurve(name))
    {
        SceneCurve *c = d_ptr->curvesMap.getCurve(name);
        d_ptr->curvesMap.remove(name);
        delete c;
    }
}

void ExtScalePlotSceneWidget::clear()
{
    d_ptr->plot->clear();
    d_ptr->curvesMap.clear();
    foreach(SceneCurve * c, d_ptr->plot->getCurves())
        removeCurve(c->name());
    d_ptr->plot->clearAxes(false, ScaleItem::Vertical);
    removeAdditionalExternalScales();
}

void ExtScalePlotSceneWidget::removeAdditionalExternalScales()
{
    QList<ExternalScaleWidget *> yscales = findChildren<ExternalScaleWidget *>();
    qDebug() << __FUNCTION__ << yscales;
    foreach(ExternalScaleWidget *scale, yscales)
    {
        if(!scale->objectName().endsWith("DefaultScale"))
        {
            qDebug() << __FUNCTION__ << "deleting scale " << scale << scale->objectName();
            d_ptr->plot->removePlotGeometryChangeListener(scale);
            delete scale;
        }
    }
}

/* NOTE: for now, only y multi scale is supported
 */
ScaleItem* ExtScalePlotSceneWidget::configureNewScale(ScaleItem::Orientation o, const QString& curveName)
{
    int axisId = d_ptr->curvesMap.getNewAxisId();
    qDebug() << __FUNCTION__ << "ADDING NEW AXIS: current x item " << d_ptr->plot->xScaleItem() ;
    ScaleItem *scaleItem = d_ptr->plot->addAxis(o, (ScaleItem::Id) axisId, d_ptr->plot->xScaleItem());
    scaleItem->setObjectName((o == ScaleItem::Horizontal ? "x: " : "y: ") + curveName);
 //  d_ptr->plot->setOriginPosPercentage(scaleItem, d_ptr->curvesMap.size() * 10.0/100.0);

    d_ptr->plot->addConfigurableObjects("Y: " + curveName, scaleItem);

    qDebug() << __FUNCTION__ << "added new scale item for " << curveName << scaleItem;

//    ExternalScaleWidget *yscale = new ExternalScaleWidget(this, ScaleItem::Vertical);
//    d_ptr->layout->addWidget(yscale, 0, 0, 10, 1);

 //   scaleItem->installAxisChangeListener(yscale);
    scaleItem->setAxisAutoscaleEnabled(true);
    scaleItem->setAxisLabelsFormat("%f");
    scaleItem->setAxisLabelsEnabled(false);
//    d_ptr->plot->installPlotGeometryChangeListener(yscale);
//    yscale->setAlignment(Qt::AlignVCenter);
//    scaleItem->setTickStepLen(100/10);
//    yscale->setTickStepLen(scaleItem->tickStepLen());
//    yscale->setOriginPercentage(d_ptr->plot->originPosPercentage(scaleItem));
//    yscale->setMargin(d_ptr->plot->frameWidth());
//    connect(d_ptr->plot->verticalScrollBar(), SIGNAL(valueChanged(int)),
//            yscale, SLOT(scrollChanged(int)));
//    QFont f = font();
//    f.setPointSize(f.pointSize() - 2);
//    yscale->setFont(f);

   // scaleItem->update();
    return scaleItem;
}

