#pragma once
#include <QObject>
#include "solver/model/input/InputData.hpp"
#include "properties/DoubleProperty.hpp"
#include "properties/IntegerProperty.hpp"
#include "properties/StringProperty.hpp"

class DataViewModel: public QObject
{
    Q_OBJECT

public:
    DataViewModel();

    void loadDefaults();
    void loadFile(const QString& path);
    void saveFile(const QString& path);

    const InputData& getData() const;
    InputData& getData();

    QString getComments() const;
    void setComments(const QString& value);

    const Settings& getSettings() const;
    void setSettings(const Settings& value);

    const Dimensions& getDimensions() const;
    void setDimensions(const Dimensions& value);

    const std::vector<Material>& getMaterials() const;
    void setMaterials(const std::vector<Material>& value);

    const std::vector<Layer>& getLayers() const;
    void setLayers(const std::vector<Layer>& value);

    const ProfileInput& getProfile() const;
    void setProfile(const ProfileInput& value);

    const std::vector<Vector<2>>& getWidth() const;
    void setWidth(const std::vector<Vector<2>>& value);

    const String& getString() const;
    void setString(const String& value);

    const Masses& getMasses() const;
    void setMasses(const Masses& value);

    const Damping& getDamping() const;
    void setDamping(const Damping& value);

signals:
    void modified();
    void reloaded();

    void commentModified();
    void settingsModified();
    void dimensionsModified();
    void materialsModified();
    void layersModified();
    void profileModified();
    void widthModified();
    void stringModified();
    void massesModified();
    void dampingModified();

private:
    InputData data;
};
