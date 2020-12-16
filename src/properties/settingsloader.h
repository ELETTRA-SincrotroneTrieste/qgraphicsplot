#ifndef SETTINGSLOADER_H
#define SETTINGSLOADER_H

#include <QString>

class QObject;

class SettingsLoader
{
public:
    SettingsLoader(const QString& settingsKey);

    void loadConfiguration(QObject *object);

private:
    QString mSettingsKey;
};

#endif // SETTINGSLOADER_H
