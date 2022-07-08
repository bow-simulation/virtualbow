#include "ViewModel.hpp"

const char* DEFAULT_NAME = "Unnamed";

ViewModel::ViewModel()
    : file_path(QString()),
      is_modified(false)
{
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::commentModified);
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::settingsModified);
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::dimensionsModified);
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::materialsModified);
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::layersModified);
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::profileModified);
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::widthModified);
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::stringModified);
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::massesModified);
    QObject::connect(this, &ViewModel::reloaded, this, &ViewModel::dampingModified);

    QObject::connect(this, &ViewModel::commentModified, this, &ViewModel::modified);
    QObject::connect(this, &ViewModel::settingsModified, this, &ViewModel::modified);
    QObject::connect(this, &ViewModel::dimensionsModified, this, &ViewModel::modified);
    QObject::connect(this, &ViewModel::materialsModified, this, &ViewModel::modified);
    QObject::connect(this, &ViewModel::layersModified, this, &ViewModel::modified);
    QObject::connect(this, &ViewModel::profileModified, this, &ViewModel::modified);
    QObject::connect(this, &ViewModel::widthModified, this, &ViewModel::modified);
    QObject::connect(this, &ViewModel::stringModified, this, &ViewModel::modified);
    QObject::connect(this, &ViewModel::massesModified, this, &ViewModel::modified);
    QObject::connect(this, &ViewModel::dampingModified, this, &ViewModel::modified);

    QObject::connect(this, &ViewModel::modified, this, [&](void* source) {
        if(source != this) {
            setModified(true);
        }
    });
}

QString ViewModel::filePath() const {
    return file_path;
}

QString ViewModel::displayPath() const {
    return file_path.isEmpty() ? DEFAULT_NAME : file_path;
}

bool ViewModel::isModified() const {
    return is_modified;
}

void ViewModel::loadDefaults() {
    data = InputData();
    emit reloaded(this);

    setFilePath(QString());
    setModified(false);
}

void ViewModel::loadFile(const QString& path) {
    data = InputData(path.toLocal8Bit().toStdString());    // toLocal8Bit() for Windows, since toStdString() would convert to UTF8
    emit reloaded(this);

    setFilePath(path);
    setModified(false);
}

void ViewModel::saveFile(const QString& path) {
    data.save(path.toLocal8Bit().toStdString());    // toLocal8Bit() for Windows, since toStdString() would convert to UTF8

    setFilePath(path);
    setModified(false);
}

void ViewModel::setFilePath(const QString& value) {
    if(file_path != value) {
        file_path = value;
        emit displayPathChanged(displayPath());
    }
}

void ViewModel::setModified(bool value) {
    if(is_modified != value) {
        is_modified = value;
        emit modificationChanged(is_modified);
    }
}

const InputData& ViewModel::getData() const {
    return data;
}

InputData& ViewModel::getData() {
    return data;
}

QString ViewModel::getComments() const {
    return QString::fromStdString(data.comment);
}

void ViewModel::setComments(const QString& value, void* source) {
    data.comment = value.toStdString();
    emit commentModified(source);
}

const Settings& ViewModel::getSettings() const {
    return data.settings;
}

void ViewModel::setSettings(const Settings& value, void* source) {
    data.settings = value;
    emit settingsModified(source);
}

const Dimensions& ViewModel::getDimensions() const {
    return data.dimensions;
}

void ViewModel::setDimensions(const Dimensions& value, void* source) {
    data.dimensions = value;
    emit dimensionsModified(source);
}

const std::vector<Material>& ViewModel::getMaterials() const {
    return data.materials;
}

void ViewModel::modifyMaterial(size_t i, const Material& material, void* source) {
    if(i < data.materials.size()) {
        data.materials[i] = material;
        emit materialModified(i, source);
        emit materialsModified(source);
    }
}

void ViewModel::insertMaterial(size_t i, const Material& material, void* source) {
    if(i <= data.materials.size()) {
        // Insert material at given index
        data.materials.insert(data.materials.begin() + i, material);
        emit materialInserted(i, source);
        emit materialsModified(source);

        // Increment references to all materials >= that index
        bool modified = false;
        for(size_t j = 0; j < data.layers.size(); ++j) {
            if(data.layers[j].material >= i) {
                data.layers[j].material += 1;
                emit layerModified(j, source);
                modified = true;
            }
        }

        if(modified) {
            emit layersModified(source);
        }
    }
}

void ViewModel::removeMaterial(size_t i, void* source) {
    if(i < data.materials.size()) {
        // Remove material at the given index
        data.materials.erase(data.materials.begin() + i);
        emit materialRemoved(i, source);
        emit materialsModified(source);

        // Remove references to the material
        bool modified = false;
        for(size_t j = 0; j < data.layers.size(); ++j) {
            if(data.layers[j].material == i) {
                data.layers[j].material = 0;
                emit layerModified(j, source);
                modified = true;
            }
        }

        if(modified) {
            emit layersModified(source);
        }
    }
}

void ViewModel::swapMaterials(size_t i, size_t j, void* source) {
    if(i != j && i < data.materials.size() && j < data.materials.size()) {
        // Swap the two materials
        std::swap(data.materials[i], data.materials[j]);
        emit materialsSwapped(i, j, source);
        emit materialsModified(source);

        // Swap references to the materials
        bool modified = false;
        for(size_t k = 0; k < data.layers.size(); ++k) {
            if(data.layers[k].material == i) {
                data.layers[k].material = j;
                emit layerModified(k, source);
                modified = true;
            }
            else if(data.layers[k].material == j) {
                data.layers[k].material = i;
                emit layerModified(k, source);
                modified = true;
            }
        }

        if(modified) {
            emit layersModified(source);
        }
    }
}

const std::vector<Layer>& ViewModel::getLayers() const {
    return data.layers;
}

void ViewModel::modifyLayer(size_t i, const Layer& layer, void* source) {
    if(i < data.layers.size()) {
        data.layers[i] = layer;
        emit layerModified(i, source);
        emit layersModified(source);
    }
}

void ViewModel::insertLayer(size_t i, const Layer& layer, void* source) {
    if(i <= data.layers.size()) {
        data.layers.insert(data.layers.begin() + i, layer);
        emit layerInserted(i, source);
        emit layersModified(source);
    }
}

void ViewModel::removeLayer(size_t i, void* source) {
    if(i < data.layers.size()) {
        data.layers.erase(data.layers.begin() + i);
        emit layerRemoved(i, source);
        emit layersModified(source);
    }
}

void ViewModel::swapLayers(size_t i, size_t j, void* source) {
    if(i != j && i < data.layers.size() && j < data.layers.size()) {
        std::swap(data.layers[i], data.layers[j]);
        emit layersSwapped(i, j, source);
        emit layersModified(source);
    }
}

const ProfileInput& ViewModel::getProfile() const {
    return data.profile;
}

void ViewModel::setProfile(const ProfileInput& value, void* source) {
    data.profile = value;
    emit profileModified(source);
}

const std::vector<Vector<2>>& ViewModel::getWidth() const {
    return data.width;
}

void ViewModel::setWidth(const std::vector<Vector<2>>& value, void* source) {
    data.width = value;
    emit widthModified(source);
}

const String& ViewModel::getString() const {
    return data.string;
}

void ViewModel::setString(const String& value, void* source) {
    data.string = value;
    emit stringModified(source);
}

const Masses& ViewModel::getMasses() const {
    return data.masses;
}

void ViewModel::setMasses(const Masses& value, void* source) {
    data.masses = value;
    emit massesModified(source);
}

const Damping& ViewModel::getDamping() const {
    return data.damping;
}

void ViewModel::setDamping(const Damping& value, void* source) {
    data.damping = value;
    emit dampingModified(source);
}
