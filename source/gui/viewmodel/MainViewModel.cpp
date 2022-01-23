#include "MainViewModel.hpp"
#include "DataViewModel.hpp"

QString DEFAULT_NAME = "Unnamed";

MainViewModel::MainViewModel()
    : data_model(new DataViewModel()),
      file_path(QString()),
      modified(false)
{
    QObject::connect(data_model, &DataViewModel::anyModified, this, [&]{
        setModified(true);
    });
}

DataViewModel* MainViewModel::dataModel() {
    return data_model;
}

QString MainViewModel::filePath() const {
    return file_path;
}

QString MainViewModel::displayPath() const {
    return file_path.isEmpty() ? DEFAULT_NAME : file_path;
}

bool MainViewModel::isModified() const {
    return modified;
}

void MainViewModel::loadDefaults() {
    data_model->loadDefaults();
    setFilePath(QString());
    setModified(false);
}

void MainViewModel::loadFile(const QString& path) {
    data_model->loadFile(path);
    setFilePath(path);
    setModified(false);
}

void MainViewModel::saveFile(const QString& path) {
    data_model->saveFile(path);
    setFilePath(path);
    setModified(false);
}

void MainViewModel::setFilePath(const QString& value) {
    if(file_path != value) {
        file_path = value;
        emit displayPathChanged(displayPath());
    }
}

void MainViewModel::setModified(bool value) {
    if(modified != value) {
        modified = value;
        emit modificationChanged(modified);
    }
}
