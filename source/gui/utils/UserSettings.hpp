#pragma once
#include <QSettings>

// Wrapper around QSettings that maintains a "version" entry and compares it against the current version.
// Resets the settings on version mismatch to prevent incompatibilities.
// The current version needs to be increased each time there is a breaking change to the settings.

class UserSettings: public QSettings {
public:
    UserSettings();

private:
    static const QString VERSION;
};
