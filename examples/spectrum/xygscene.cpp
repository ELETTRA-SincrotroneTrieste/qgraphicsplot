#include "xygscene.h"
#include "../../src/circles/circleitemset.h"
#include "curve/painters/linepainter.h"
#include "curve/painters/dotspainter.h"
#include "../../src/colors.h"
#include "curve/curveitem.h"
#include "ui_xygscene.h"

#include <QTimer>
#include <QtAlgorithms>
#include <stdio.h>
#include <QtDebug>
#include <math.h>
#include "items/markeritem.h"

#include "curve/scenecurve.h"

XYSceneTest::XYSceneTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("Spectrum Example");
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
                                                   KDARKPINK << KVERYDARKVIOLET;

    /* x axis will have at most bufsiz points */
    ui->graphicsPlot->xScaleItem()->setBounds(0, bufsiz);
    ui->graphicsPlot->yScaleItem()->setBounds(-10, 10.0);

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

    //    DotsPainter *dp = new DotsPainter(curveItem);
    //    dp->setDotsColor(palette.at(i % palette.size()));
    //    dp->setBorderColor(dp->dotsColor().darker());

//        curveItem->setFineBoundingRectCalculationEnabled(true);
       // ui->graphicsPlot->addConfigurableObjects(c->name() + " Properties", curveItem);
    }

    /* the following instructions start the timer to update the data */
    timer->setSingleShot(false);
    timer->setInterval(ui->sbPeriod->value());
    connect(timer, SIGNAL(timeout()), this, SLOT(createData()));
    timer->start();

    connect(ui->sbPeriod, SIGNAL(valueChanged(int)), timer, SLOT(start(int)));
    connect(ui->dsbScaleX, SIGNAL(valueChanged(double)), this, SLOT(scale()), Qt::QueuedConnection);
    connect(ui->dsbScaleY, SIGNAL(valueChanged(double)), this, SLOT(scale()), Qt::QueuedConnection);

    mCnt = 0;

    MarkerItem* marker = new MarkerItem(0);
    ui->graphicsPlot->installMouseEventListener(marker);
    ui->graphicsPlot->scene()->addItem(marker);

    x1 = 0.0;
    y1 = 0.0;
    x2 = 0.0;
    y2 = -10.0;

    /* install the CircleItemSet change event listener on the view */
    ///  CircleItemSet *circleItemSet = new  CircleItemSet(this, mCurve1->bufferSize());
    ///  mCurve1->installItemChangeListener(circleItemSet);
    ///  ui->graphicsView->addConfigurableObjects("Circle Painter", circleItemSet);

    /* load from configuration files (QSettings managed) */
    ui->graphicsPlot->loadConfigurationProperties();


   // ui->graphicsPlot->yScaleItem()->setAxisAutoscaleEnabled(false);
   // ui->graphicsPlot->xScaleItem()->setAxisAutoscaleEnabled(false);
}

XYSceneTest::~XYSceneTest()
{
    delete ui;
}

void XYSceneTest::scale()
{
    ui->graphicsPlot->scale(ui->dsbScaleX->value(), ui->dsbScaleY->value());
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
        amplitude = qrand() / (double) RAND_MAX * maxAmplitude;
        QVector<double> xData, yData;

        for(int j = 0; j < buf; j++)
        {
            x = j / (double) ui->sbPrecision->value();
            y = sin(x) * amplitude;
            xData << x;
            yData << y;

        }
        c->setData(xData, yData);
    }
}

