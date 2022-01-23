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
