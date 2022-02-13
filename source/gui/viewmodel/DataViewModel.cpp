#include "DataViewModel.hpp"
#include "gui/units/UnitSystem.hpp"

DataViewModel::DataViewModel() {
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::commentModified);
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::settingsModified);
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::dimensionsModified);
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::materialsModified);
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::layersModified);
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::profileModified);
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::widthModified);
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::stringModified);
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::massesModified);
    QObject::connect(this, &DataViewModel::reloaded, this, &DataViewModel::dampingModified);

    QObject::connect(this, &DataViewModel::commentModified, this, &DataViewModel::modified);
    QObject::connect(this, &DataViewModel::settingsModified, this, &DataViewModel::modified);
    QObject::connect(this, &DataViewModel::dimensionsModified, this, &DataViewModel::modified);
    QObject::connect(this, &DataViewModel::materialsModified, this, &DataViewModel::modified);
    QObject::connect(this, &DataViewModel::layersModified, this, &DataViewModel::modified);
    QObject::connect(this, &DataViewModel::profileModified, this, &DataViewModel::modified);
    QObject::connect(this, &DataViewModel::widthModified, this, &DataViewModel::modified);
    QObject::connect(this, &DataViewModel::stringModified, this, &DataViewModel::modified);
    QObject::connect(this, &DataViewModel::massesModified, this, &DataViewModel::modified);
    QObject::connect(this, &DataViewModel::dampingModified, this, &DataViewModel::modified);

}

void DataViewModel::loadDefaults() {
    data = InputData();
    emit reloaded();
}

void DataViewModel::loadFile(const QString& path) {
    data = InputData(path.toStdString());
    emit reloaded();
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

QString DataViewModel::getComments() const {
    return QString::fromStdString(data.comment);
}

void DataViewModel::setComments(const QString& value) {
    data.comment = value.toStdString();
    emit commentModified();
}

const Settings& DataViewModel::getSettings() const {
    return data.settings;
}

void DataViewModel::setSettings(const Settings& value) {
    data.settings = value;
    emit settingsModified();
}

const Dimensions& DataViewModel::getDimensions() const {
    return data.dimensions;
}

void DataViewModel::setDimensions(const Dimensions& value) {
    data.dimensions = value;
    emit dimensionsModified();
}

const std::vector<Material>& DataViewModel::getMaterials() const {
    return data.materials;
}

void DataViewModel::setMaterials(const std::vector<Material>& value) {
    data.materials = value;
    emit materialsModified();
}

const std::vector<Layer>& DataViewModel::getLayers() const {
    return data.layers;
}

void DataViewModel::setLayers(const std::vector<Layer>& value) {
    data.layers = value;
    emit layersModified();
}

const ProfileInput& DataViewModel::getProfile() const {
    return data.profile;
}

void DataViewModel::setProfile(const ProfileInput& value) {
    data.profile = value;
    emit profileModified();
}

const std::vector<Vector<2>>& DataViewModel::getWidth() const {
    return data.width;
}

void DataViewModel::setWidth(const std::vector<Vector<2>>& value) {
    data.width = value;
    emit widthModified();
}

const String& DataViewModel::getString() const {
    return data.string;
}

void DataViewModel::setString(const String& value) {
    data.string = value;
    emit stringModified();
}

const Masses& DataViewModel::getMasses() const {
    return data.masses;
}

void DataViewModel::setMasses(const Masses& value) {
    data.masses = value;
    emit massesModified();
}

const Damping& DataViewModel::getDamping() const {
    return data.damping;
}

void DataViewModel::setDamping(const Damping& value) {
    data.damping = value;
    emit dampingModified();
}
