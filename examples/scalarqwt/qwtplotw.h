#ifndef XYGSCENE_H
#define XYGSCENE_H

#include <QWidget>

class EPlotCurve;
namespace Ui {
class Widget;
}

class EPlotLightTest : public QWidget
{
    Q_OBJECT
    
public:
    explicit EPlotLightTest(QWidget *parent = 0);
    ~EPlotLightTest();
    

public slots:

private slots:
    void createData();

    void precisionChanged(int);

    void bufferChanged(int);

private:
    Ui::Widget *ui;

    int mCnt;

    QList<EPlotCurve *> curves;

    QStringList curveNames;

    double x1, y1, x2, y2;
};

#endif // XYGSCENE_H
