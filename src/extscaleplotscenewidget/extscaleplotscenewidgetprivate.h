#ifndef EXTSCALEPLOTSCENEWIDGETPRIVATE_H
#define EXTSCALEPLOTSCENEWIDGETPRIVATE_H

class QGridLayout;
class QHBoxLayout;
class PlotSceneWidget;

#include "curvesmap.h"

class ExtScalePlotSceneWidgetPrivate
{
public:
    ExtScalePlotSceneWidgetPrivate() {}

    QGridLayout *layout;
    QHBoxLayout *yLeftLo, *yRightLo;
    PlotSceneWidget *plot;
    bool multiscale;
    CurvesMap curvesMap;
};

#endif // EXTSCALEPLOTSCENEWIDGETPRIVATE_H
