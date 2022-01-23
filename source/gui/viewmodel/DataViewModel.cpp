#include "DataViewModel.hpp"

DataViewModel::DataViewModel() {
    QObject::connect(this, &DataViewModel::allModified, this, &DataViewModel::commentsModified);
    QObject::connect(this, &DataViewModel::allModified, this, &DataViewModel::settingsModified);
    QObject::connect(this, &DataViewModel::allModified, this, &DataViewModel::dimensionsModified);

    QObject::connect(this, &DataViewModel::commentsModified, this, &DataViewModel::anyModified);
    QObject::connect(this, &DataViewModel::settingsModified, this, &DataViewModel::anyModified);
    QObject::connect(this, &DataViewModel::dimensionsModified, this, &DataViewModel::anyModified);
}

void DataViewModel::loadDefaults() {
    setData(InputData(), this);
}

void DataViewModel::loadFile(const QString& path) {
    setData(InputData(path.toStdString()), this);
}

void DataViewModel::saveFile(const QString& path) {
    data.save(path.toStdString());
}

const InputData& DataViewModel::getData() const {
    return data;
}

void DataViewModel::setData(const InputData& data, QObject* source) {
    this->data = data;
    emit allModified(source);
}

QString DataViewModel::getComments() const {
    return QString::fromStdString(data.comment);
}

void DataViewModel::setComments(const QString& comment, QObject* source) {
    data.comment = comment.toStdString();
    emit commentsModified(source);
}
