#ifndef XYGSCENE_H
#define XYGSCENE_H

#include <QWidget>

class CircleItemSet;
class SceneCurve;

namespace Ui {
class Widget;
}

class ExternalScales : public QWidget
{
    Q_OBJECT
    
public:
    explicit ExternalScales(QWidget *parent = 0);
    ~ExternalScales();

private slots:
    void createData();

private:
    Ui::Widget *ui;

    int mCnt;

    QList<SceneCurve *> curves;

    double x1, y1, x2, y2;
};

#endif // XYGSCENE_H
