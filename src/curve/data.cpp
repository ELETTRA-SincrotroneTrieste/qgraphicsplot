#include "data.h"
#include "scenecurve.h"
#include "../qgraphicsplotmacros.h"
#include <math.h>

#include <QtDebug>

using namespace std;

Data::Data()
{
    lastValidXPos = lastValidYPos = -1;
    xMinMaxUnset = yMinMaxUnset = true;
    mXDataChanged = mYDataChanged = false;
    xMin = xMax = 0.0;
    yMin = yMax = 0.0;
    scalarMode = true;
    xDataOrdered = true;
    yDataOrdered = false;
}

Data::~Data()
{
    printf("\e[1;31mdata destroyed\e[0m\n");
}

void Data::resetMaxMin()
{
    xMinMaxUnset = yMinMaxUnset = true;
}

void Data::setData(const QVector<double> &vx, const QVector<double> &vy)
{
    scalarMode = false;
    if(vx != xData)
    {
        lastValidXPos = -1;
        mXDataChanged = true;
        xData = vx;
    }
    else
        mXDataChanged = false;

    if(vy != yData)
    {
        lastValidYPos = -1;
        mYDataChanged = true;
        yData = vy;
    }
    else
        mYDataChanged = false;
}

void Data::setData(const QVector<double> &yDat)
{
    int dataSize = yData.size();
    scalarMode = false;
    if(xData.size() != dataSize)
    {
        for(int i = 0; i < dataSize; i++)
            xData  << i;
        mXDataChanged = true;
    }
    yData = yDat;
    /* suppose yData changes */
    mYDataChanged = true;
}

/** \brief Returns a vector of double containing the abscissa values whose Y values
 *         are NaN.
 *
 * @return a vector of double with the x values associated to a NaN Y value
 */
QVector<double> Data::invalidDataPoints() const
{
    QVector<double> xinvalid;
    int xsiz = xData.size();
    int ysiz = yData.size();
    for(int i = 0; i < xData.size() && xsiz == ysiz; i++)
        if(isnan(yData.at(i)))
            xinvalid << xData.at(i);
    return xinvalid;
}

void Data::addPoints(const QVector<double> &xData, const QVector<double> &yData)
{
    int xsiz = xData.size();
    int ysiz = yData.size();
    for(int i = 0; i < xData.size() && xsiz == ysiz; i++)
        addPoint(xData.at(i), yData.at(i));
}

void Data::addPoint(double x, double y)
{
    scalarMode = true;
    /* update max and min each time a point is added. It's free!
     * Don't update max and min if x or y are NaN
     */
    if(!isnan(x))
    {
        if(xMinMaxUnset)
        {
            xMin = xMax = x;
            xMinMaxUnset = false;
        }
        if(x < xMin)
            xMin = x;
        else if(x > xMax)
            xMax = x;
    }
    if(!isnan(y))
    {
        if(yMinMaxUnset)
        {
            yMinMaxUnset = false;
            yMin = yMax = y;
        }


        if(y < yMin)
            yMin = y;
        else if(y > yMax)
            yMax = y;
    }

    xData.append(x);
    yData.append(y);

    mYDataChanged = true;
    mXDataChanged = true;
}

Point Data::point(int index) const
{
    return Point(xData[index], yData[index]);
}

void Data::remove(int index)
{
    if(index < xData.size())
    {
        xData.remove(index);
        yData.remove(index);
    }
}

int Data::size() const
{
    return xData.size();
}

void Data::calculateXBounds()
{
    if(size() <= 0)
        return;

    int i = 0;
    xMin = xMax = 0.0;
    if(xDataOrdered)
    {
        for(i = 0; i < xData.size() && isnan(xData[i]); i++)
            /* skip NaNs */ ;
        if(i < xData.size())
            xMin = xData[i];

        for(i = xData.size() - 1; i >= 0 && isnan(xData[i]); i--)
            /* skip NaNs */ ;
        if(i >= 0)
            xMax = xData[i];
        return;
    }

    /* find first not NaN value */
    for(i = 0; i < xData.size() && isnan(xData[i]); i++)
        /* skip */  ;
    if(i < xData.size())
    {
        xMin = xMax = xData[i];

        for(int k = i + 1; k < size(); k++)
        {
            double x = xData[k];
            if(!isnan(x))
            {
                /* update max and mins if necessary */
                if(x > xMax)
                    xMax = x;
                else if(x < xMin)
                    xMin = x;
            }
        }
    }
}

void Data::calculateYBounds()
{
    if(size() <= 0)
        return;

    int i = 0;
    yMin = yMax = 0.0;

    if(yDataOrdered)
    {
        for(i = 0; i < yData.size() && isnan(yData[i]); i++)
            /* skip NaNs */ ;
        if(i < yData.size())
            yMin = yData[i];

        for(i = yData.size() - 1; i >= 0 && isnan(yData[i]); i--)
            /* skip NaNs */ ;
        if(i >= 0)
            yMax = yData[i];
        return;
    }

    /* find first not NaN value */
    for(i = 0; i < yData.size() && isnan(yData[i]); i++)
        /* skip */  ;
    if(i < yData.size())
    {
        yMin =yMax = yData[i];

        for(int k = i + 1; k < size(); k++)
        {
            double y = yData[k];
            if(!isnan(y))
            {
                /* update max and mins if necessary */
                if(y > yMax)
                    yMax = y;
                else if(y < yMin)
                    yMin = y;
            }
        }
    }
}

void Data::calculateBounds()
{
    if(size() <= 0)
        return;

    xMin = yMin = 0.0;
    xMax = yMax = 0.0;

    int yi, xi, i;

    if(xDataOrdered)
    {
        for(i = 0; i < xData.size() && isnan(xData[i]); i++)
            /* skip NaNs */ ;
        if(i < xData.size())
            xMin = xData[i];

        for(i = xData.size() - 1; i >= 0 && isnan(xData[i]); i--)
            /* skip NaNs */ ;
        if(i >= 0)
            xMax = xData[i];

        calculateYBounds();
        return;
    }

    if(yDataOrdered)
    {
        for(i = 0; i < yData.size() && isnan(yData[i]); i++)
            /* skip NaNs */ ;
        if(i < yData.size())
            yMin = yData[i];

        for(i = yData.size() - 1; i >= 0 && isnan(yData[i]); i--)
            /* skip NaNs */ ;
        if(i >= 0)
            yMax = yData[i];

        calculateXBounds();
        return;
    }

    for(yi = 0; yi < yData.size() && isnan(yData[yi]); yi++)
        /* skip nans in y */  ;

    for(xi = 0; xi < xData.size() && isnan(xData[xi]); xi++)
        /* skip NaNs in x */  ;

    if(xi < xData.size())
        xMin = xMax = xData[xi]; /* not NaN */
    if(yi < yData.size())
        yMin = yMax = yData[yi]; /* not NaN */

    for(int i = 0; i < size(); i++) /* start from 0 */
    {
        double x = xData[i];
        double y = yData[i];
        /* update max and mins if necessary */
        if(!isnan(x) && x > xMax)
            xMax = x;
        else if(!isnan(x) && x < xMin)
            xMin = x;

        if(!isnan(y) && y > yMax)
            yMax = y;
        else if(!isnan(y) && y < yMin)
            yMin = y;
    }
}

bool Data::dataUnchanged() const
{
    return !mXDataChanged && !mYDataChanged;
}

void Data::cacheData()
{
    mXDataChanged = false;
    mYDataChanged = false;
}
