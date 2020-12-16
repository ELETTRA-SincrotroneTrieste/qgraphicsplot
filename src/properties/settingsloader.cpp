#include "settingsloader.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QSettings>
#include <qgraphicsplotmacros.h>
#include <QtDebug>

SettingsLoader::SettingsLoader(const QString &settingsKey)
{
    mSettingsKey = settingsKey;
    if(settingsKey.isEmpty())
        perr("SettingsLoader::SettingsLoader(): \e[1;31mempty key: please provide a valid key\e[0m");
}

void SettingsLoader::loadConfiguration(QObject *object)
{
    QSettings s;
    if(mSettingsKey.isEmpty())
        perr("SettingsLoader::loadConfiguration: settings key is empty. Cannot load properties for \"%s\"",
             qstoc(object->objectName()));
    if(object->objectName().isEmpty())
        perr("SettingsLoader::loadConfiguration: object name is empty");

    if(mSettingsKey.isEmpty() || object->objectName().isEmpty())
    {
        perr("SettingsLoader::loadConfiguration: cannot load properties for obj. \"%s\", class \"%s\"",
             qstoc(object->objectName()), object->metaObject()->className());
        return;
    }

    QString key;
    bool ok;
    const QString key1 = mSettingsKey + "/" + object->objectName() + "/";
    const QMetaObject *mo = object->metaObject();
    int cnt = 0;
    int propertyConfigurationSuperclass = object->property("__propertyConfigurationSuperclass").toInt(&ok);
    if(!ok)
        propertyConfigurationSuperclass = 1;

    while(cnt < propertyConfigurationSuperclass)
    {
        for(int i = mo->propertyOffset(); i < mo->propertyCount(); i++)
        {
            QMetaProperty mp = mo->property(i);
            key = key1 + mp.name();
            if(s.contains(key))
            {
                ok = mp.write(object, s.value(key));
                if(!ok)
                    perr("SettingsLoader::loadConfiguration: failed to write property \"%s\" "
                         "on object \"%s\" class \"%s\"", qstoc(key), qstoc(object->objectName()),
                         mo->className());
            }
        }
        mo = mo->superClass();
        cnt++;
    }
}
