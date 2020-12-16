#ifndef AGINGCIRCLES_H
#define AGINGCIRCLES_H

#include <QWidget>

class CircleItemSet;
class SceneCurve;

namespace Ui {
class Widget;
}

class AgingCircles : public QWidget
{
    Q_OBJECT
    
public:
    explicit AgingCircles(QWidget *parent = 0);
    ~AgingCircles();
    

public slots:

private slots:
    void createData();

private:
    Ui::Widget *ui;

    int mCnt;

    QList<SceneCurve *> curves;

    double x1, y1, x2, y2;
};

#endif // AGINGCIRCLES_H
