#pragma once
#include <QObject>
#include "solver/model/input/InputData.hpp"

class ViewModel: public QObject
{
    Q_OBJECT

public:
    ViewModel();

    QString filePath() const;
    QString displayPath() const;
    bool isModified() const;

    void loadDefaults();
    void loadFile(const QString& path);
    void saveFile(const QString& path);

    const InputData& getData() const;
    InputData& getData();

    QString getComments() const;
    void setComments(const QString& value, void* source);

    const Settings& getSettings() const;
    void setSettings(const Settings& value, void* source);

    const Dimensions& getDimensions() const;
    void setDimensions(const Dimensions& value, void* source);

    const std::vector<Material>& getMaterials() const;
    void setMaterials(const std::vector<Material>& value, void* source);

    const std::vector<Layer>& getLayers() const;
    void setLayers(const std::vector<Layer>& value, void* source);

    const ProfileInput& getProfile() const;
    void setProfile(const ProfileInput& value, void* source);

    const std::vector<Vector<2>>& getWidth() const;
    void setWidth(const std::vector<Vector<2>>& value, void* source);

    const String& getString() const;
    void setString(const String& value, void* source);

    const Masses& getMasses() const;
    void setMasses(const Masses& value, void* source);

    const Damping& getDamping() const;
    void setDamping(const Damping& value, void* source);

signals:
    void displayPathChanged(const QString& path);
    void modificationChanged(bool modified);

    void modified(void* source);
    void reloaded(void* source);

    void commentModified(void* source);
    void settingsModified(void* source);
    void dimensionsModified(void* source);
    void materialsModified(void* source);
    void layersModified(void* source);
    void profileModified(void* source);
    void widthModified(void* source);
    void stringModified(void* source);
    void massesModified(void* source);
    void dampingModified(void* source);

private:
    InputData data;
    QString file_path;
    bool is_modified;

    void setFilePath(const QString& value);
    void setModified(bool value);
};
