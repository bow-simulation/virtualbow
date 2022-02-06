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

Material& DataViewModel::createMaterial(int index, const QString& name) {
    Material material;
    material.name = name.toStdString();    // TODO: Give material a constructir for this

    data.materials.insert(data.materials.begin() + index, material);    // TODO: No duplicate names?
    return data.materials[index];
}

void DataViewModel::removeMaterial(int index) {
    data.materials.erase(data.materials.begin() + index);
}

void DataViewModel::swapMaterials(int i, int j) {
    std::swap(data.materials[i], data.materials[j]);
}

Layer& DataViewModel::createLayer(int index, const QString& name) {
    Layer layer;
    layer.name = name.toStdString();    // TODO: Give layer a constructor for this

    data.layers.insert(data.layers.begin() + index, layer);
    return data.layers[index];
}

void DataViewModel::removeLayer(int index) {
    data.layers.erase(data.layers.begin() + index);
}

void DataViewModel::swapLayers(int i, int j) {
    std::swap(data.layers[i], data.layers[j]);
}

void DataViewModel::addSegment(int index, const SegmentInput& segment) {
    data.profile.insert(data.profile.begin() + index, segment);
}

void DataViewModel::removeSegment(int index) {
    data.profile.erase(data.profile.begin() + index);
}
void DataViewModel::swapSegments(int i, int j) {
    std::swap(data.profile[i], data.profile[j]);
}
