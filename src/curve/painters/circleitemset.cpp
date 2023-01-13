
#include <QtDebug>
#include "circleitemset.h"
#include "curve/point.h"
#include "scenecurve.h"
#include "curveitem.h"
#include "colors.h"
#include <QtAlgorithms>
#include <stdio.h>
#include <math.h>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

CircleItemSet::~CircleItemSet()
{
    mCurveItem->removeItemPainterInterface(this);
}

CircleItemSet::CircleItemSet(CurveItem *curveIt, int bufferSize,
                             qreal maxRadius, int numDifferentRadius)
    : QObject(curveIt)
{
    mCurveItem = curveIt;
    mBufferSize = bufferSize;
    mMaxRadius = maxRadius;
    mRadiusScaleDivider = 1.0;
    mNumDifferentRadius = numDifferentRadius;
    mAlphaChannel = 255;
    mBorderEnabled = true;
    mGradientEnabled = false;
    setObjectName("CircleItemSet");
    curveIt->installItemPainterInterface(this);
    /// test
  //  setColorList(QList<QColor>() << KLIGHTGRAY << KYELLOW << KORANGE << KRED << KVIOLET <<  KCYAN << KBLUE << KVERYDARKBLUE );
    setColorList(QList<QColor>() << KLIGHTGRAY << Qt::black);
    //  setColorList(QList<QColor>() << QColor(0, 0, 5) <<QColor(0, 0, 100) << QColor(0, 0, 200));
}

int CircleItemSet::type() const
{
    return ItemPainterInterface::CircleItemSet;
}

void CircleItemSet::draw(SceneCurve *curve,
                  QPainter *painter,
                  const QStyleOptionGraphicsItem * option,
                  QWidget * widget)
{
    Q_UNUSED(widget);
    int circleStep, radius;
    int itemCnt = curve->dataSize();
    if(itemCnt == 0)
        return;

    int changeEvery = mBufferSize / mNumDifferentRadius + 1;

    circleStep = qRound(mMaxRadius / mNumDifferentRadius);

    int nStepsPerInterval = qRound((float)(mBufferSize) /  (float)(mColorList.size() - 1) + 0.5);

//    printf("\e[1;32m new item: %d %d %d --------------------------------------------\e[0m\n",
//           mBufferSize, mColorList.size(), nStepsPerInterval);
    const QPointF *points = curve->points();

    painter->save();
    /* clip painter */
    painter->setClipRect(option->exposedRect);
    for(int i = 0; i < itemCnt; i++)
    {
        radius = mMaxRadius - ((itemCnt - i) / changeEvery ) * circleStep;
        radius /= mRadiusScaleDivider;
        QPointF point = points[i];
        mCurveItem->setZValue(i);
        QColor circleColor = mCalculateColor(i, itemCnt, nStepsPerInterval);
        circleColor.setAlpha(mAlphaChannel);
        if(mBorderEnabled)
        {

        }
        if(mGradientEnabled)
        {

        }
        if(mAlphaChannel)
        {

        }

        painter->setBrush(circleColor);
        painter->drawEllipse(point, radius, radius);
    }
    painter->restore();
}

CurveItem *CircleItemSet::curveItem() const
{
    return mCurveItem;
}

QSizeF CircleItemSet::elementSize() const
{
    return QSizeF(2 * mMaxRadius / mRadiusScaleDivider, 2 * mMaxRadius / mRadiusScaleDivider);
}

void CircleItemSet::bufferSizeChanged(int newSize)
{
    mBufferSize = newSize;
}

void CircleItemSet::setGradientEnabled(bool en)
{
    mGradientEnabled = en;
}

bool CircleItemSet::gradientEnabled() const
{
    return mGradientEnabled;
}

void CircleItemSet::setBorderEnabled(bool en)
{
    mBorderEnabled = en;
}

bool CircleItemSet::borderEnabled() const
{
    return mBorderEnabled;
}

QColor CircleItemSet::mCalculateColor(int i, int nMax, int nStepsPerInterval) const
{
    Q_UNUSED(nMax);
    QColor ret = KBLUE;
    int nColors = mColorList.size();
    if(nColors > 1)
    {
        int changeEvery = nStepsPerInterval / nStepsPerInterval;
        /* qMin(): do not exceed color list dimensions: this might happen when i == nMax
         * Since  this is invoked by  itemAboutToBeAdded() it happens that nMax takes into
         * account one extra item that will soon be removed.
         */
        //int intervalIdx = nIntervals - 1 - qMin(nIntervals * i / (mBufferSize + 2), nIntervals -1); /* 0, 1, ..., nColors - 1 */

        int colorIdx = (i-1) / nStepsPerInterval;

        qreal q = ((i-1) / changeEvery) % nStepsPerInterval;


//        printf("\e[0;32mnColors %d changeEvery %d colorIdx %d (%d/%d) stepsPerInterval %d\e[1;31m index %.1f\e[0m\n",
//               nColors,
//               changeEvery, colorIdx, i, nMax, nStepsPerInterval, q);

        QColor c1, c2;
        if(colorIdx + 1 < mColorList.size())
            c2 = mColorList.at(colorIdx + 1);
        else
            c2 = mColorList.last();

        if(colorIdx < mColorList.size())
            c1 = mColorList.at(colorIdx);
        else
            c1 = mColorList.first();

        qreal c1r, c1g, c1b, c2r, c2g, c2b;
        /* get color 1 and 2 floating point values */
        c1r = c1.redF();
        c1g = c1.greenF();
        c1b = c1.blueF();
        c2r = c2.redF();
        c2g = c2.greenF();
        c2b = c2.blueF();

        qreal red, green, blue;

        qreal delta = c2r - c1r;
        /* red */
        qreal increment = delta / nStepsPerInterval;
        red = c1r + q * increment;

        /* green */
        delta = c2g - c1g;
        increment = delta / nStepsPerInterval;
        green = c1g +  q * increment;


        /* blue */
        delta = c2b - c1b;
        increment = delta / nStepsPerInterval ;
        blue = c1b +  q * increment;

        ret.setRgbF(red, green, blue);

       // qDebug() << c1 << c2 << ret << red << green << blue;
    }
    return ret;
}


