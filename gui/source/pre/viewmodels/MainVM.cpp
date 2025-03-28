#include "MainVM.hpp"
#include "ModelTreeVM.hpp"
#include "solver/API.hpp"
#include <QItemSelectionModel>
#include <QAbstractItemModelTester>

MainVM::MainVM():
    bow(std::nullopt),
    path(""),
    unsaved(false),
    modelTreeVM(new ModelTreeVM()),
    modelTreeSelectionVM(new QItemSelectionModel(modelTreeVM))
{
    // TODO: Move this to a dedicated test
    // new QAbstractItemModelTester(modelTreeVM, QAbstractItemModelTester::FailureReportingMode::Warning, this);
}

ModelTreeVM* MainVM::getModelTreeVM() {
    return modelTreeVM;
}

QItemSelectionModel* MainVM::getModelTreeSelectionVM() {
    return modelTreeSelectionVM;
}

void MainVM::newFile() {
    // Create  default bow data, which is not linked to any file yet
    this->bow = new_model();
    this->modelTreeVM->setBowModel(&bow.value());
    this->path = "";
    this->unsaved = false;

    emit currentFileChanged(path);
    emit hasBowModelChanged(true);
    emit hasUnsavedWorkChanged(unsaved);
}

void MainVM::loadFile(const QString& path) {
    // Load bow data from file, which becomes the current path.
    this->bow = load_model(path.toStdString(), true);
    this->path = path;
    this->unsaved = false;
    this->modelTreeVM->setBowModel(&bow.value());

    emit currentFileChanged(path);
    emit hasBowModelChanged(true);    // Might not have actually changed, but doesn't really matter
    emit hasUnsavedWorkChanged(unsaved);
}

void MainVM::saveFile(const QString& path) {
    // If bow data is available, save it to the file, which becomes the current path.
    if(bow.has_value()) {
        save_model(bow.value(), path.toStdString());
        this->path = path;
        this->unsaved = false;

        emit currentFileChanged(path);
        emit hasUnsavedWorkChanged(unsaved);
    }
}

const QString& MainVM::currentFile() const {
    return path;
}

bool MainVM::hasBowModel() const {
    return bow.has_value();
}

bool MainVM::hasUnsavedWork() const {
    return unsaved;
}
