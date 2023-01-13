#ifndef SCALEITEMPRIVATE_H
#define SCALEITEMPRIVATE_H

#include "scaleitem.h"
#include <QPen>
#include <QList>
#include <QRectF>
#include <QFont>
#include <QMap>

class AxisChangeListener;
class ScaleLabelInterface;

class ScaleItemPrivate
{
public:
    ScaleItemPrivate();

    ScaleItem::Orientation orientation;

    qreal tickWidth;

    qreal tickStepLen, actualTickStepLen, tickStepLenScale;

    qreal fontSize;

    bool labelsEnabled;

    qreal axisLabelRotation;

    QGraphicsPlotItem *plot_item;

    qreal mLastTickPos;

    QString axisLabelsFormat,  actualLabelsFormat, axisTitle;

    bool gridEnabled, autoScale;

    QColor gridColor, axisColor, axisTitleColor;

    double mAxisTitleWidth, mAxisTitleHeight;

    bool mNeedFullRedraw;

    ScaleItem::Id axisId;

    double upperBound, lowerBound, autoscaleMargin;

    double axisLabelDist;

    bool minMaxUnset;

    bool axisLabelsOutsideCanvas;

    QList<AxisChangeListener* > axisChangeListeners;

    ScaleLabelInterface *scaleLabelInterface;

    QRectF plotRect, canvasRect;

    double maxLabelWidth, labelMargin, labelHeight;

    QString longestLabel;

    QMap<double, QString> labelsCacheHash;

    QFont font, axisTitleFont;

    int plotZoomLevel;
};



#endif // SCALEITEMPRIVATE_H
