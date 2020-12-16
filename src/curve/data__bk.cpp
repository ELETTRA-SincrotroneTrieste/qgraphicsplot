#include "data.h"
#include "scenecurve.h"
#include "../qgraphicsplotmacros.h"
#include "datachangelistener.h"

Data::Data()
{
    lastValidXPos = lastValidYPos = -1;
    xMinMaxUnset = yMinMaxUnset = true;
    mXDataUnchanged = mYDataUnchanged = false;
    mPoints = NULL;
    xMin = xMax = 0.0;
    yMin = yMax = 0.0;
}

Data::~Data()
{
    printf("\e[1;31mdata destroyed\e[0m\n");
}

void Data::installDataChangeListener(DataChangeListener *l)
{
    dataChangeListeners.append(l);
}

void Data::removeDataChangeListener(DataChangeListener *l)
{
    dataChangeListeners.removeAll(l);
}

void Data::resetMaxMin()
{
    xMinMaxUnset = yMinMaxUnset = true;
}

void Data::setData(const QVector<double> &vx, const QVector<double> &vy)
{
    if(vx != mXData)
    {
        lastValidXPos = -1;
        mXDataUnchanged = false;
        mXData = vx;
    }
    else
        mXDataUnchanged = true;

    if(vy != mYData)
    {
        lastValidYPos = -1;
        mYDataUnchanged = false;
        mYData = vy;
    }
    else
        mYDataUnchanged = true;

    int dataSize = vy.size();
    if(xPos.size() != dataSize)
    {
        xPos.resize(dataSize);
        yPos.resize(dataSize);
    }
    if(!mXDataUnchanged || !mYDataUnchanged)
    {
        /* invalidate points cache */
        delete mPoints;
        mPoints = NULL;
    }
}

void Data::setData(const QVector<double> &yData)
{
    int dataSize = yData.size();
    if(mXData.size() != dataSize)
    {
        for(int i = 0; i < dataSize; i++)
            mXData  << i;
    }
    mYData = yData;
    if(xPos.size() != dataSize)
    {
        xPos.resize(dataSize);
        yPos.resize(dataSize);
    }
}

void Data::addPoint(double x, double y)
{
    mXData.append(x);
    mYData.append(y);
    if(xPos.size() < mYData.size())
    {
        xPos.resize(xPos.size() + 10);
        yPos.resize(yPos.size() + 10);
    }
}

Point Data::point(int index) const
{
    return Point(mXData[index], mYData[index]);
}

void Data::remove(int index)
{
    if(index < mXData.size())
    {
        mXData.remove(index);
        mYData.remove(index);
    }
    if(index < xPos.size())
    {
        xPos.remove(index);
        xPos.remove(index);
    }
}

void Data::cachePlotRect(const QRectF &plotR)
{
    plotRectTop = plotR.top();
    plotRectW = plotR.width();
    plotRectH = plotR.height();
    plotRectLeft = plotR.left();
}

void Data::cacheXBounds(double xl, double xu)
{
    xub = xu;
    xlb = xl;
    xextension = xub - xlb;
}

void Data::cacheYBounds(double yl, double yu)
{
    yub = yu;
    ylb = yl;
    yextension = yub - ylb;
}

int Data::size() const
{
    return mXData.size();
}

void Data::calculateXBounds()
{
    if(size() <= 0)
        return;
    if(xMinMaxUnset)
        xMin = xMax = mXData[0];

    for(int i = 0; i < size(); i++)
    {
        double x = mXData[i];
        /* update max and mins if necessary */
        if(x > xMax)
           xMax = x;
        else if(x < xMin)
            xMin = x;
    }
}

void Data::calculateYBounds()
{
    if(size() <= 0)
        return;

    if(yMinMaxUnset)
        yMin = yMax = mYData[0];
    for(int i = 0; i < size(); i++)
    {
        double y = mYData[i];
        /* update max and mins if necessary */
        if(y > yMax)
            yMax = y;
        else if(y < yMin)
            yMin = y;
    }
}

void Data::calculateBounds()
{
    if(size() <= 0)
        return;
    if(xMinMaxUnset)
        xMin = xMax = mXData[0];
    if(yMinMaxUnset)
        yMin = yMax = mYData[0];

    for(int i = 0; i < size(); i++)
    {
        double x = mXData[i];
        double y = mYData[i];
        /* update max and mins if necessary */
        if(x > xMax)
           xMax = x;
        else if(x < xMin)
            xMin = x;

        if(y > yMax)
            yMax = y;
        else if(y < yMin)
            yMin = y;
    }
}

QPointF *Data::points()
{
    if(mPoints) /* data did not change */
        return mPoints;

    if(size() <= 0)
        return NULL;

    mPoints = new QPointF[size()];

    int index = -1;
    for(index = 0; index < size(); index++)
    {
        double xp = 0.0;
        if(index < mXData.size())
        {
            if(lastValidXPos >= index && lastValidXPos > -1)
            {
                printf("--- using pos in index %d for X\n", index);
                xp = xPos[index];
            }
            else
            {
                     printf("--- must recalc X pos cuz idx %d lastValidX %d\n", index, lastValidXPos);
                double x = mXData[index];
                if(xub == xlb)
                    return NULL;
                xp = plotRectW * (x - xlb) / (xextension) + plotRectLeft;
                xPos[index] = xp; /* cache calculated value */
            }
        }
        else
            perr("Data::xpos: index %d outside bounds (%d)", index, mXData.size());

        double yp = 0.0;
        if(index < mYData.size())
        {
            if(lastValidYPos >= index && lastValidYPos > -1)
            {
                printf("--- using pos in index %d for Y\n", index);
                yp = yPos[index];
            }
            else
            {
                printf("--- must recalc Y pos cuz idx %d lastValidX %d\n", index, lastValidXPos);
                double y = mYData[index];
                if(yub == ylb)
                    return NULL;
                yp = plotRectH - (plotRectH * (y - ylb) / (yextension) + plotRectTop);
                yPos[index] = yp;  /* cache calculated value */
            }
        }
        else
            perr("Data::ypos: index %d outside bounds (%d)", index, mYData.size());

        mPoints[index] = QPointF(xp, yp);
    }
    /* we made the data object calculate all the positions for its points.
     * From the curve point of view, all its points positions are determined
     * We mark the x and y scene coordinates positions valid.
     */
    lastValidXPos = index - 1;
    lastValidYPos = index - 1;
    return mPoints;
}

double Data::xpos(int index)
{
    double xp = 0.0;
    if(index < mXData.size())
    {
        if(lastValidXPos >= index && lastValidXPos > -1)
        {
            ///printf("--- using pos in index %d for X\n", index);
            return xPos[index];
        }
      ///  else
       ///     printf("--- must recalc X pos cuz idx %d lastValidX %d\n", index, lastValidXPos);

        double x = mXData[index];
        if(xub == xlb)
            return xp;
        xp = plotRectW * (x - xlb) / (xextension) + plotRectLeft;
        xPos[index] = xp; /* cache calculated value */
    }
    else
        perr("Data::xpos: index %d outside bounds (%d)", index, mXData.size());

    return xp;
}

double Data::ypos(int index)
{
    double yp = 0.0;
    if(index < mYData.size())
    {
        if(lastValidYPos >= index && lastValidYPos > -1)
        {
       ///     printf("--- using pos in index %d for Y\n", index);
            return yPos[index];
        }
    ///    else
      ///      printf("--- must recalc Y pos cuz idx %d lastValidX %d\n", index, lastValidXPos);
        double y = mYData[index];
        if(yub == ylb)
            return yp;
       yp = plotRectH - (plotRectH * (y - ylb) / (yextension) + plotRectTop);
        yPos[index] = yp;  /* cache calculated value */
    }
    else
        perr("Data::ypos: index %d outside bounds (%d)", index, mYData.size());
    return yp;
}


void Data::invalidateXData()
{
   /// printf("\e[1;31minvalidating x\e[0m\n");
    lastValidXPos = -1;
}

void Data::invalidateYData()
{
   /// printf("\e[1;31minvalidating y\e[0m\n");
    lastValidYPos = -1;
}

