#include "qgraphicsplotplugin.h"
#include <QtPlugin>

#include "plotscenewidget.h"
#include "horizontalscalewidget.h"
#include "verticalscalewidget.h"

QGraphicsPlotPluginCustomWidgetInterface::QGraphicsPlotPluginCustomWidgetInterface(QObject *parent) :
    QObject(parent)

{
  d_isInitialized = false;
}

void QGraphicsPlotPluginCustomWidgetInterface::initialize(QDesignerFormEditorInterface *)
{
    if (d_isInitialized)
        return;

    d_isInitialized = true;
}

ExternalHorizontalScaleWidgetPlugin::ExternalHorizontalScaleWidgetPlugin(QObject *parent): QGraphicsPlotPluginCustomWidgetInterface(parent)
{
    d_name = "HorizontalScaleWidget";
    d_include = "horizontalscalewidget.h";
    d_icon = QPixmap(":pixmaps/horizontalscale.png");
    d_domXml =
        "<widget class=\"HorizontalScaleWidget\" name=\"horizontalScaleWidget\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>450</width>\n"
        "   <height>30</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *ExternalHorizontalScaleWidgetPlugin::createWidget(QWidget *parent)
{
    return new HorizontalScaleWidget(parent);
}

ExternalVerticalScaleWidgetPlugin::ExternalVerticalScaleWidgetPlugin(QObject *parent): QGraphicsPlotPluginCustomWidgetInterface(parent)
{
    d_name = "VerticalScaleWidget";
    d_include = "verticalscalewidget.h";
    d_icon = QPixmap(":pixmaps/verticalscale.png");
    d_domXml =
        "<widget class=\"VerticalScaleWidget\" name=\"verticalScaleWidget\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>30</width>\n"
        "   <height>300</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *ExternalVerticalScaleWidgetPlugin::createWidget(QWidget *parent)
{
    return new VerticalScaleWidget(parent);
}

PlotSceneWidgetPlugin::PlotSceneWidgetPlugin(QObject *parent): QGraphicsPlotPluginCustomWidgetInterface(parent)
{
    d_name = "PlotSceneWidget";
    d_include = "plotscenewidget.h";
    d_icon = QPixmap(":pixmaps/qgraphicsplot2.png");
    d_domXml =
        "<widget class=\"PlotSceneWidget\" name=\"plotSceneWidget\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>450</width>\n"
        "   <height>300</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *PlotSceneWidgetPlugin::createWidget(QWidget *parent)
{
    return new PlotSceneWidget(parent);
}

CustomWidgetCollectionInterface::CustomWidgetCollectionInterface(QObject *parent): QObject(parent)
{
    d_plugins.append(new PlotSceneWidgetPlugin(this) );
    d_plugins.append(new ExternalHorizontalScaleWidgetPlugin(this));
    d_plugins.append(new ExternalVerticalScaleWidgetPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> CustomWidgetCollectionInterface::customWidgets(void) const
{
    return d_plugins;
}

#if QT_VERSION < 0x050000

Q_EXPORT_PLUGIN2(QGraphicsPlot, CustomWidgetCollectionInterface)

#endif

