#include "externalscales.h"
#include "curve/painters/linepainter.h"
#include "curve/painters/dotspainter.h"
#include "../../src/colors.h"
#include "curve/curveitem.h"
#include "ui_externalscales.h"

#include <QTimer>
#include <QtAlgorithms>
#include <QScrollBar>
#include <stdio.h>
#include <QtDebug>
#include <math.h>
#include "items/markeritem.h"
#include <QStyleOption>

#include "curve/scenecurve.h"

ExternalScales::ExternalScales(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("External Scales Example");
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
    ui->plot->getPlot()->setSettingsKey(qApp->applicationName());

    QList<QColor> palette = QList<QColor> () << KDARKWATER << KDARKBLUE << KGRAY <<
                                                KYELLOW << KCAMEL << KDARKCYAN <<
                                                KDARKPINK << KVERYDARKVIOLET;

    /* create the curves */
    for(int i = 0; i < nCurves; i++)
    {
        /* each curve MUST have a name */
        QString name = QString("Curve %1").arg(i + 1);
        /* SceneCurve manages items in the curve. It does not draw anything */
        SceneCurve *c = ui->plot->addCurve(name, NULL, NULL);
        /* set the buffer size */
        c->setBufferSize(bufsiz);
        /* add the reference to the new curve to the list of curves */
        curves << c;
        /* update each curve buffer size when the spin box changes */
        connect(ui->sbBuffer, SIGNAL(valueChanged(int)), c, SLOT(setBufferSize(int)));
        /* each curve can be configured by the property dialog, so we must add each
         * curve to the list of configurable objects
         */
        ui->plot->getPlot()->addConfigurableObjects(c->name(), c);

        /* curves represented by lines */
        CurveItem *curveItem = new CurveItem(c);
        ui->plot->getPlot()->scene()->addItem(curveItem);
        c->installCurveChangeListener(curveItem);
        LinePainter *lp = new LinePainter(curveItem);
        lp->setColor(palette.at(i % palette.size()));
    }

    /* the following instructions start the timer to update data */
    timer->setSingleShot(false);
    timer->setInterval(ui->sbPeriod->value());
    connect(timer, SIGNAL(timeout()), this, SLOT(createData()));
    timer->start();

    connect(ui->sbPeriod, SIGNAL(valueChanged(int)), timer, SLOT(start(int)));

    /* a marker appears when  the user clicks near a curve */
    MarkerItem* marker = new MarkerItem(0);
    ui->plot->getPlot()->installMouseEventListener(marker);
    ui->plot->getPlot()->scene()->addItem(marker);

    /* load from configuration files (QSettings managed) */
    ui->plot->getPlot()->loadConfigurationProperties();
    /* x axis will have at most bufsiz points */
    ui->plot->getPlot()->xScaleItem()->setBounds(0, bufsiz);
    ui->plot->getPlot()->yScaleItem()->setBounds(-10, 10.0);
}

ExternalScales::~ExternalScales()
{
    delete ui;
}

void ExternalScales::createData()
{
    double amplitude;
    double maxAmplitude;
    double baseAmplitude = 10;
    double x, y;
    int buf = ui->sbBuffer->value();
    for(int i = 0; i < curves.size() ; i++)
    {
        SceneCurve *c = curves.at(i);
        maxAmplitude = baseAmplitude * (i+1);
        amplitude = qrand() / (double) RAND_MAX * maxAmplitude;
        QVector<double> xData, yData;

        for(int j = 0; j < buf / (i+1); j++)
        {
            x = j / (double) ui->sbPrecision->value() * 2 * M_PI * (i + 1);
            y = i * maxAmplitude + sin(x) * amplitude;
            xData << x;
            yData << y;
        }
        qDebug() << __FUNCTION__ << "setData";
        c->setData(xData, yData);
    }
}

