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
    void modifyMaterial(size_t i, const Material& material, void* source);
    void insertMaterial(size_t i, const Material& material, void* source);
    void removeMaterial(size_t i, void* source);
    void swapMaterials(size_t i, size_t j, void* source);

    const std::vector<Layer>& getLayers() const;
    void modifyLayer(size_t i, const Layer& layer, void* source);
    void insertLayer(size_t i, const Layer& layer, void* source);
    void removeLayer(size_t i, void* source);
    void swapLayers(size_t i, size_t j, void* source);

    const ProfileInput& getProfile() const;
    void modifySegment(size_t i, const SegmentInput& segment, void* source);
    void insertSegment(size_t i, const SegmentInput& segment, void* source);
    void removeSegment(size_t i, void* source);
    void swapSegments(size_t i, size_t j, void* source);

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
    void materialModified(size_t i, void* source);
    void materialInserted(size_t i, void* source);
    void materialRemoved(size_t i, void* source);
    void materialsSwapped(size_t i, size_t j, void* source);

    void layersModified(void* source);
    void layerModified(size_t i, void* source);
    void layerInserted(size_t i, void* source);
    void layerRemoved(size_t i, void* source);
    void layersSwapped(size_t i, size_t j, void* source);

    void profileModified(void* source);
    void segmentModified(size_t i, void* source);
    void segmentInserted(size_t i, void* source);
    void segmentRemoved(size_t i, void* source);
    void segmentsSwapped(size_t i, size_t j, void* source);

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
