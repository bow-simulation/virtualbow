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
    emit allModified();
}

void DataViewModel::loadFile(const QString& path) {
    data = InputData(path.toStdString());
    emit allModified();
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

QString DataViewModel::get_comments() const {
    return QString::fromStdString(data.comment);
}

void DataViewModel::set_comments(const QString& value) {
    data.comment = value.toStdString();
    emit commentModified();
}

int DataViewModel::get_limb_elements() const {
    return data.settings.n_limb_elements;
}

void DataViewModel::set_limb_elements(int value) {
    data.settings.n_limb_elements = value;
    emit settingsModified();
}

int DataViewModel::get_string_elements() const {
    return data.settings.n_string_elements;
}

void DataViewModel::set_string_elements(int value) {
    data.settings.n_string_elements = value;
    emit settingsModified();
}

int DataViewModel::get_draw_steps() const {
    return data.settings.n_draw_steps;
}

void DataViewModel::set_draw_steps(int value) {
    data.settings.n_draw_steps = value;
    emit settingsModified();
}

double DataViewModel::get_arrow_clamp_force() const {
    return data.settings.arrow_clamp_force;
}

void DataViewModel::set_arrow_clamp_force(double value) {
    data.settings.arrow_clamp_force = value;
    emit settingsModified();
}

double DataViewModel::get_time_span_factor() const {
    return data.settings.time_span_factor;
}

void DataViewModel::set_time_span_factor(double value) {
    data.settings.time_span_factor = value;
    emit settingsModified();
}

double DataViewModel::get_time_step_factor() const {
    return data.settings.time_step_factor;
}

void DataViewModel::set_time_step_factor(double value) {
    data.settings.time_step_factor = value;
    emit settingsModified();
}

double DataViewModel::get_sampling_rate() const {
    return data.settings.sampling_rate;
}

void DataViewModel::set_sampling_rate(double value) {
    data.settings.sampling_rate = value;
    emit settingsModified();
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
