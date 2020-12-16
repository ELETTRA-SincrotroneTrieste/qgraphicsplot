#include "scalartime.h"
#include "../../src/colors.h"
#include "curve/painters/linepainter.h"
#include "curve/curveitem.h"
#include "ui_scalar.h"

#include <QTimer>
#include <QtAlgorithms>
#include <QDateTime>
#include <stdio.h>
#include <QtDebug>
#include <math.h>
#include "scalelabels/timescalelabel.h"
#include "curve/scenecurve.h"
#include "items/markeritem.h"

XYSceneTest::XYSceneTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    int nCurves = 1;
    int bufsiz = 1000;
    int speed = 500;

    QTimer *timer = new QTimer(this);
    bool ok;
    if(qApp->arguments().size() > 3 && qApp->arguments().at(1).toInt(&ok) && ok)
    {
        nCurves = qApp->arguments().at(1).toInt();
        bufsiz = qApp->arguments().at(2).toInt();
        speed = qApp->arguments().at(3).toInt();
    }
    else
    {
        qDebug() << "usage .." << qApp->arguments().at(0) << " nCurves bufsiz refreshRate[ms]";
        exit(EXIT_FAILURE);
    }

    ui->sbBuffer->setValue(bufsiz);
    ui->sbPeriod->setValue(speed);

    /* setup a settings key to be able to edit and save the properties
     * of the plot and of the related objects.
     */
    qDebug() << "setto settings key " << qApp->applicationName();
    ui->graphicsPlot->setSettingsKey(qApp->applicationName());

    QList<QColor> palette = QList<QColor> () << KDARKWATER << KDARKBLUE << KGRAY <<
                                                KYELLOW << KCAMEL << KDARKCYAN <<
                                                KDARKPINK << KVERYDARKVIOLET << KORANGE
                                             << KVERYDARKBLUE << KVERYDARKYELLOW
                                             << KPINKPIG << KGREEN << KDARKRED
                                             << KDARKMAROON << KDARKGREEN
                                             << KDARKGRAY;

    /* x axis will have at most bufsiz points */
    TimeScaleLabel *timeScaleLabel = new TimeScaleLabel();
    connect(ui->cbShowDate, SIGNAL(toggled(bool)), this, SLOT(setShowDate(bool)));
    ui->graphicsPlot->xScaleItem()->installScaleLabelInterface(timeScaleLabel);

    ui->graphicsPlot->xScaleItem()->setBounds(-10, -9);
    ui->graphicsPlot->yScaleItem()->setBounds(-1.2, 1.2);

    connect(ui->teLower, SIGNAL(dateTimeChanged(QDateTime)), ui->graphicsPlot->xScaleItem(),
            SLOT(setLowerBoundDateTime(QDateTime)));
    connect(ui->teUpper, SIGNAL(dateTimeChanged(QDateTime)), ui->graphicsPlot->xScaleItem(),
            SLOT(setUpperBoundDateTime(QDateTime)));
    connect(ui->pbNow, SIGNAL(clicked()), this, SLOT(setNow()));

    connect(ui->cbAutoscale, SIGNAL(toggled(bool)), ui->graphicsPlot->xScaleItem(),
            SLOT(setAxisAutoscaleEnabled(bool)));

    /* autoscale enabled by default */
    ui->cbAutoscale->setChecked(true);

    ui->graphicsPlot->yScaleItem()->setAxisAutoscaleEnabled(true);

    /* signals are connected, this will change the scales */
    ui->teUpper->setDateTime(QDateTime::currentDateTime().addSecs(10));
    ui->teLower->setDateTime(QDateTime::currentDateTime().addSecs(-2));

    MarkerItem* marker = new MarkerItem(0);
    ui->graphicsPlot->installMouseEventListener(marker);
    ui->graphicsPlot->scene()->addItem(marker);

    /* create the curves */
    for(int i = 0; i < nCurves; i++)
    {
        /* each curve MUST have a name */
        QString name = QString("Curve %1").arg(i + 1);
        /* SceneCurve manages items in the curve. It does not draw anything */
        SceneCurve *c = ui->graphicsPlot->addCurve(name);
        /* set the buffer size */
        c->setBufferSize(bufsiz);

        /* add the reference to the new curve to the list of curves */
        curves << c;
        /* update each curve buffer size when the spin box changes */
        connect(ui->sbBuffer, SIGNAL(valueChanged(int)), c, SLOT(setBufferSize(int)));
        /* each curve can be configured by the property dialog, so we must add each
         * curve to the list of configurable objects
         */
        // ui->graphicsPlot->addConfigurableObjects(c->name(), c);

        /* do we want the curves be represented by lines? */
        CurveItem *curveItem = new CurveItem(c);
        ui->graphicsPlot->scene()->addItem(curveItem);
        c->installCurveChangeListener(curveItem);
        LinePainter *lp = new LinePainter(curveItem);
        lp->setLineColor(palette.at(i % palette.size()));
        // ui->graphicsPlot->addConfigurableObjects(c->name() + " Properties", curveItem);
    }

    /* the following instructions start the timer to update the data */
    timer->setSingleShot(false);
    timer->setInterval(ui->sbPeriod->value());
    connect(timer, SIGNAL(timeout()), this, SLOT(createData()));
    timer->start();

    connect(ui->sbPeriod, SIGNAL(valueChanged(int)), timer, SLOT(start(int)));

    mCnt = 0;



    x1 = 0.0;
    y1 = 0.0;
    x2 = 0.0;
    y2 = -10.0;

    /* install the CircleItemSet change event listener on the view */
    ///  CircleItemSet *circleItemSet = new  CircleItemSet(this, mCurve1->bufferSize());
    ///  mCurve1->installItemChangeListener(circleItemSet);
    ///  ui->graphicsView->addConfigurableObjects("Circle Painter", circleItemSet);

    /* external scales configuration */
    ui->graphicsPlot->xScaleItem()->installAxisChangeListener(ui->xScale);
    ui->graphicsPlot->yScaleItem()->installAxisChangeListener(ui->yScale);
    ui->graphicsPlot->installPlotGeometryChangeListener(ui->xScale);
    ui->graphicsPlot->installPlotGeometryChangeListener(ui->yScale);

    ui->xScale->setAlignment(Qt::AlignHCenter);
    ui->yScale->setAlignment(Qt::AlignVCenter);

    /* by deafault, the ExternalScaleWidget is synchronized with the
     * PlotSceneWidget scales tick step len and labels format.
     */
    ui->graphicsPlot->yScaleItem()->setTickStepLen(0.1);
    ui->graphicsPlot->yScaleItem()->setAxisLabelsFormat("%.2f");
    ui->graphicsPlot->xScaleItem()->setTickStepLen(-1);

    /* time scale draw on external scale */
    ui->xScale->installScaleLabelInterface(timeScaleLabel);
    ui->xScale->setLabelRotation(60);

    /* correctly align the external scale with the plot grid and scales */
    ui->yScale->setOriginPercentage(0.5); /* according to plot's */
    ui->xScale->setOriginPercentage(0.5);

    /* do we want to be able to configure the ExternalScaleWidget properties from our
     * application?
     */
    ui->graphicsPlot->addConfigurableObjects("X Scale", ui->xScale);
    ui->graphicsPlot->addConfigurableObjects("Y Scale", ui->yScale);

    qDebug() << "nomi scale " << ui->xScale->objectName() << ui->yScale->objectName();


    /* load from configuration files (QSettings managed) */
    ui->graphicsPlot->loadConfigurationProperties();
}

XYSceneTest::~XYSceneTest()
{
    delete ui;
}

void XYSceneTest::setNow()
{
    ui->teUpper->setDateTime(QDateTime::currentDateTime().addSecs(ui->sbAddSecs->value()));
    ui->cbAutoscale->setChecked(false);
}

void XYSceneTest::setShowDate(bool en)
{
    static_cast<TimeScaleLabel *>(ui->graphicsPlot->xScaleItem()->scaleLabelInterface())
            ->setShowDate(en);
}

void XYSceneTest::createData()
{
    double amplitude;
    double maxAmplitude;
    double x, y;
    int buf = ui->sbBuffer->value();
    for(int i = 0; i < curves.size() ; i++)
    {
        SceneCurve *c = curves.at(i);
        maxAmplitude = 1 + i * 2;
        amplitude = qrand() / (double) RAND_MAX * maxAmplitude - maxAmplitude/2.0;
        x1 += 3 / (double) 100;
        y = sin(x1) * maxAmplitude;
        y = -1 + mCnt % 3;
        c->addPoint(QDateTime::currentDateTime().toTime_t(), y);
    }
    mCnt++;
}

