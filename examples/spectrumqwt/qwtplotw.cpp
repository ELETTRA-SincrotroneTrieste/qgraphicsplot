#include "qwtplotw.h"
#include "ui_qwtplotw.h"

#include <QtDebug>
#include <QTimer>
#include <QtAlgorithms>
#include <stdio.h>
#include <math.h>
#include <elettracolors.h>
#include <eplotcurve.h>

EPlotLightTest::EPlotLightTest(QWidget *parent) :
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
        qDebug() << "usage " << qApp->arguments().at(0) << " nCurves bufsiz refreshRate[ms]";
        exit(EXIT_FAILURE);
    }

    ui->sbBuffer->setValue(bufsiz);
    ui->sbPeriod->setValue(speed);
    bufferChanged(bufsiz); /* corrects the scale*/

    /* create the curves */
    for(int i = 0; i < nCurves; i++)
    {
        /* each curve MUST have a name */
        QString name = QString("Curve %1").arg(i + 1);
        /* SceneCurve manages items in the curve. It does not draw anything */
        EPlotCurve *c = new EPlotCurve(this);
        ui->plot->addCurve(name, c);
        curves << c;
        curveNames << name;
    }

    /* the following instructions start the timer to update the data */
    timer->setSingleShot(false);
    timer->setInterval(ui->sbPeriod->value());
    connect(timer, SIGNAL(timeout()), this, SLOT(createData()));
    timer->start();

    connect(ui->sbPeriod, SIGNAL(valueChanged(int)), timer, SLOT(start(int)));

    connect(ui->sbBuffer, SIGNAL(valueChanged(int)), this, SLOT(bufferChanged(int)));
    connect(ui->sbPrecision, SIGNAL(valueChanged(int)), this, SLOT(precisionChanged(int)));

    mCnt = 0;



    x1 = 0.0;
    y1 = 0.0;
    x2 = 0.0;
    y2 = -10.0;

    ui->plot->setXAutoscale(true);
    ui->plot->setYAutoscale(true);

}

void EPlotLightTest::precisionChanged(int p)
{
    double xUb = ui->sbBuffer->value()/p;
    ui->plot->setXUpperBound(xUb);
}

void EPlotLightTest::bufferChanged(int s )
{
    double xUb = s/(double)ui->sbPrecision->value();
    ui->plot->setXUpperBound(xUb);
}

EPlotLightTest::~EPlotLightTest()
{
    delete ui;
}


void EPlotLightTest::createData()
{
    double amplitude;
    double maxAmplitude;
    double x, y;
    int buf = ui->sbBuffer->value();
    for(int i = 0; i < curveNames.size() ; i++)
    {
        QVector<double> xData, yData;
        QString curveName = curveNames.at(i);
        maxAmplitude = 1 + i * 2;
        amplitude = qrand() / (double) RAND_MAX * maxAmplitude;

        for(int j = 0; j < buf; j++)
        {
            x = j / (double) ui->sbPrecision->value();
            xData << x;
            y = sin(x) * amplitude;
            yData << y;
        }
        ui->plot->setData(curveName, xData, yData);
    }
    ui->plot->refresh();
}

