#pragma once
#include <QtCore>

// Wrapper around QSettings to load and save all values at once

class UserSettings
{
private:
    QMap<QString, QVariant> data;

public:
    void load()
    {
        QSettings settings;
        for(const QString& key: settings.allKeys())
            data[key] = settings.value(key);
    }

    void save()
    {
        QSettings settings;
        for(const QString& key: data.keys())
            settings.setValue(key, data[key]);
    }

    void setValue(const QString& key, const QVariant& value)
    {
        data[key] = value;
    }

    QVariant getValue(const QString& key, const QVariant& default_value = QVariant()) const
    {
        return data.contains(key) ? data[key] : default_value;
    }
};
