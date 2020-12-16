#ifndef PROPERTYDIALOG_H
#define PROPERTYDIALOG_H

#include <QDialog>
#include <QMap>

class PlotSceneWidget;

class PropertyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PropertyDialog(PlotSceneWidget *sceneWidget);

    virtual ~PropertyDialog();

    void configureTabs(const QMap<QString, QObject*> objectsMap) { mObjectsMap = objectsMap; }

    void setSettingsKey(const QString& key) { mSettingsKey = key; }
    
signals:
    
public slots:
    
public slots:
    void show();

private:
    QMap<QString, QObject*> mObjectsMap;

    QString mSettingsKey;

    QString mGetQSettingsKey(QObject *o, const QString &propName) const;

private slots:
    void propertyChanged();

    void chooseColorButton();

    void clearSettings();

    void execFontDialog();

    void itemSelectionChanged();

};

#endif // PROPERTYDIALOG_H
