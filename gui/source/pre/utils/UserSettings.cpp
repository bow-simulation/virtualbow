#include "UserSettings.hpp"

// Currently accepted version of the user settings
const QString UserSettings::VERSION = "0";

UserSettings::UserSettings() {
    if(value("version") != VERSION) {
        clear();
        setValue("version", VERSION);
    }
}
