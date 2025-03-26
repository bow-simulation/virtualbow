#include "MainVM.hpp"
#include "ModelTreeVM.hpp"
#include "solver/API.hpp"

#pragma once
#include <QObject>
#include <QProperty>
#include <QTimer>

MainVM::MainVM():
    path(""),
    unsaved(false),
    modelTreeVM(new ModelTreeVM())
{

}

ModelTreeVM* MainVM::getModelTreeVM() {
    return modelTreeVM;
}

void MainVM::newFile() {
    this->path = "";
    this->unsaved = false;

    emit currentFileChanged(path);
    emit unsavedWorkChanged(unsaved);
}

void MainVM::loadFile(const QString& path) {
    //this->modelTreeVM->setBowModel(load_model(path.toStdString(), true));
    this->path = path;
    this->unsaved = false;

    emit currentFileChanged(path);
    emit unsavedWorkChanged(unsaved);
}

void MainVM::saveFile(const QString& path) {
    //save_model(this->modelTreeVM->getBowModel(), path.toStdString());
    this->path = path;
    this->unsaved = false;

    emit currentFileChanged(path);
    emit unsavedWorkChanged(unsaved);
}

const QString& MainVM::currentFile() const {
    return path;
}

bool MainVM::hasUnsavedWork() const {
    return unsaved;
}
