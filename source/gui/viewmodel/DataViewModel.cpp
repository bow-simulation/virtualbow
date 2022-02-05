#include "DataViewModel.hpp"

DataViewModel::DataViewModel() {
    QObject::connect(this, &DataViewModel::allModified, this, &DataViewModel::commentModified);
    QObject::connect(this, &DataViewModel::allModified, this, &DataViewModel::settingsModified);
    QObject::connect(this, &DataViewModel::allModified, this, &DataViewModel::dimensionsModified);

    QObject::connect(this, &DataViewModel::commentModified, this, &DataViewModel::anyModified);
    QObject::connect(this, &DataViewModel::settingsModified, this, &DataViewModel::anyModified);
    QObject::connect(this, &DataViewModel::dimensionsModified, this, &DataViewModel::anyModified);
}

void DataViewModel::loadDefaults() {
    data = InputData();
    emit allModified(this);
}

void DataViewModel::loadFile(const QString& path) {
    data = InputData(path.toStdString());
    emit allModified(this);
}

void DataViewModel::saveFile(const QString& path) {
    data.save(path.toStdString());
}

const InputData& DataViewModel::getData() const {
    return data;
}

InputData& DataViewModel::getData() {
    return data;
}

Material& DataViewModel::addMaterial(int index) {
    data.materials.insert(data.materials.begin() + index, Material());    // TODO: No duplicate names
    return data.materials[index];
}

void DataViewModel::removeMaterial(int index) {
    auto it = data.materials.begin() + index;
    data.materials.erase(it);
}

void DataViewModel::swapMaterials(int i, int j) {
    std::swap(data.materials[i], data.materials[j]);
}

Layer& DataViewModel::addLayer(int index) {
    data.layers.insert(data.layers.begin() + index, Layer());    // TODO: No duplicate names
    return data.layers[index];
}

void DataViewModel::removeLayer(int index) {
    auto it = data.layers.begin() + index;
    data.layers.erase(it);
}

void DataViewModel::swapLayers(int i, int j) {
    std::swap(data.layers[i], data.layers[j]);
}
