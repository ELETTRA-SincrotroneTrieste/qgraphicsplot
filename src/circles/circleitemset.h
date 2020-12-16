#ifndef CIRCLEITEMSET_H
#define CIRCLEITEMSET_H

#include "itempainterinterface.h"
#include <QObject>
#include <QColor>
#include <QList>

class CircleItemSet : public QObject, public ItemPainterInterface
{
    Q_PROPERTY(int numDifferentRadius READ numDifferentRadius WRITE setNumDifferentRadius)
    Q_PROPERTY(double maxRadius READ maxRadius WRITE setMaxRadius)
    Q_PROPERTY(int alphaChannel READ alphaChannel WRITE setAlphaChannel)
    Q_PROPERTY(bool gradientEnabled READ gradientEnabled WRITE setGradientEnabled)
    Q_PROPERTY(bool borderEnabled READ borderEnabled WRITE setBorderEnabled)

    Q_OBJECT
public:

    CircleItemSet(CurveItem *curveItem, int bufferSize,
                  qreal maxRadius = 50,
                  int numDifferentRadius = 10);
public:

    QList<QColor> colorList() const { return mColorList; }

    int bufferSize() const { return mBufferSize; }

    int numDifferentRadius() const { return mNumDifferentRadius; }

    double maxRadius() const { return mMaxRadius; }

    int alphaChannel() const { return mAlphaChannel; }

    void setGradientEnabled(bool en);

    bool gradientEnabled() const;

    void setBorderEnabled(bool en);

    bool borderEnabled() const;

    virtual void bufferSizeChanged(int newSize);


    virtual void draw(SceneCurve *curve,
                      PlotSceneWidget* plot,
                      QPainter *painter,
                      const QStyleOptionGraphicsItem * option,
                      QWidget * widget = 0);

    virtual CurveItem *curveItem() const;

    virtual QSizeF elementSize() const;

public slots:

    void setMaxRadius(double r) { mMaxRadius = r; }

    void setNumDifferentRadius(int nr) { mNumDifferentRadius = nr; }

    void setBufferSize(int bSize) { mBufferSize = bSize; }

    void setColorList(const QList<QColor>& colors) { mColorList = colors; }

    void setAlphaChannel(int alpha) { mAlphaChannel = alpha; }


private:

    QList<QColor> mColorList;

    int mBufferSize, mNumDifferentRadius, mAlphaChannel;

    double mMaxRadius;

    QColor mCalculateColor(int idx, int nMax, int nStepsPerInterval) const;

    bool mBorderEnabled, mGradientEnabled;

    CurveItem *mCurveItem;
};

#endif // CIRCLEITEMSET_H
