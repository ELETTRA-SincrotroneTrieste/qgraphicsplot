#ifndef XYGSCENE_H
#define XYGSCENE_H

#include <QWidget>

class CircleItemSet;
class SceneCurve;

namespace Ui {
class Widget;
}

class XYSceneTest : public QWidget
{
    Q_OBJECT
    
public:
    explicit XYSceneTest(QWidget *parent = 0);
    ~XYSceneTest();
    

public slots:

private slots:
    void createData();

    void scale();

private:
    Ui::Widget *ui;

    int mCnt;

    QList<SceneCurve *> curves;

    double x1, y1, x2, y2;
};

#endif // XYGSCENE_H
