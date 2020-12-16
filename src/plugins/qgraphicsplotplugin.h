#ifndef QGRAPHICSPLOTPLUGINCUSTOMWIDGETINTERFACE_H
#define QGRAPHICSPLOTPLUGINCUSTOMWIDGETINTERFACE_H

#include <QObject>
#if QT_VERSION >= 0x050500
	#include <QtUiPlugin/QDesignerCustomWidgetInterface>
#else
	#include <QtDesigner/QDesignerCustomWidgetInterface>
#endif

class QGraphicsPlotPluginCustomWidgetInterface :
        public QObject,
        public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    QGraphicsPlotPluginCustomWidgetInterface(QObject *parent = 0);

    virtual bool isContainer() const { return false; }
    virtual bool isInitialized() const { return d_isInitialized; }
    virtual QIcon icon() const { return d_icon; }
    virtual QString codeTemplate() const { return d_codeTemplate; }
    virtual QString domXml() const { return d_domXml; }
    virtual QString group() const { return "QGraphicsPlot"; }
    virtual QString includeFile() const { return d_include; }
    virtual QString name() const { return d_name; }
    virtual QString toolTip() const { return d_toolTip; }
    virtual QString whatsThis() const { return d_whatsThis; }


    virtual void initialize(QDesignerFormEditorInterface *);

protected:
    QString d_name;
    QString d_include;
    QString d_toolTip;
    QString d_whatsThis;
    QString d_domXml;
    QString d_codeTemplate;
    QIcon d_icon;

private:
    bool d_isInitialized;
signals:
    
public slots:
    
};


class CustomWidgetCollectionInterface: public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

#if QT_VERSION >= 0x050000
        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
#endif

public:
    CustomWidgetCollectionInterface(QObject *parent = NULL);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> d_plugins;
};

class ExternalHorizontalScaleWidgetPlugin : public QGraphicsPlotPluginCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    ExternalHorizontalScaleWidgetPlugin(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class ExternalVerticalScaleWidgetPlugin : public QGraphicsPlotPluginCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    ExternalVerticalScaleWidgetPlugin(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class PlotSceneWidgetPlugin: public QGraphicsPlotPluginCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    PlotSceneWidgetPlugin(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};



#endif // QGRAPHICSPLOTPLUGINCUSTOMWIDGETINTERFACE_H
