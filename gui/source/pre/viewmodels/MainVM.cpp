#include "MainVM.hpp"
#include "ModelTreeVM.hpp"
#include "CommentsVM.hpp"
#include "SettingsVM.hpp"
#include "DimensionsVM.hpp"
#include "MaterialVM.hpp"
#include "LayerVM.hpp"
#include "StringVM.hpp"
#include "MassesVM.hpp"
#include "DampingVM.hpp"
#include "LineVM.hpp"
#include "ArcVM.hpp"
#include "SpiralVM.hpp"
#include "pre/widgets/TableModel.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "solver/API.hpp"
#include <QItemSelectionModel>
#include <QAbstractItemModelTester>

MainVM::MainVM():
    bow(std::nullopt),
    path(""),
    unsaved(false),
    modelTreeVM(new ModelTreeVM(this)),
    modelTreeSelectionVM(new QItemSelectionModel(modelTreeVM))
{
    // TODO: Move this to a dedicated test
    // new QAbstractItemModelTester(modelTreeVM, QAbstractItemModelTester::FailureReportingMode::Warning, this);

    //connectSubModel(modelTreeVM);

    QObject::connect(this, &MainVM::contentModified, this, [&]{
        if(!unsaved) {
            unsaved = true;
            emit hasUnsavedWorkChanged(true);
        }
    });
}

ModelTreeVM* MainVM::getModelTreeVM() {
    return modelTreeVM;
}

QItemSelectionModel* MainVM::getModelTreeSelectionVM() {
    return modelTreeSelectionVM;
}

CommentsVM* MainVM::getCommentsVM() {
    if(bow.has_value()) {
        return new CommentsVM(this, bow->comment);
    }

    return nullptr;
}

SettingsVM* MainVM::getSettingsVM() {
    if(bow.has_value()) {
        return new SettingsVM(this, bow->settings);
    }

    return nullptr;
}

DimensionsVM* MainVM::getDimensionsVM() {
    if(bow.has_value()) {
        return new DimensionsVM(this, bow->dimensions);
    }

    return nullptr;
}

MaterialVM* MainVM::getMaterialVM(int index) {
    if(bow.has_value() && index >= 0 && index < bow->materials.size()) {
        return new MaterialVM(this, bow->materials[index]);
    }

    return nullptr;
}

LayerVM* MainVM::getLayerVM(int index) {
    if(bow.has_value() && index >= 0 && index < bow->layers.size()) {
        return new LayerVM(this, bow->layers[index], bow->materials);
    }

    return nullptr;
}

TableModel* MainVM::getLayerHeightVM(int index) {
    if(bow.has_value() && index >= 0 && index < bow->layers.size()) {
        return new TableModel(this, bow->layers[index].height, "Position", "Height", Quantities::ratio, Quantities::length);
    }

    return nullptr;
}

TableModel* MainVM::getWidthVM() {
    if(bow.has_value()) {
        return new TableModel(this, bow->width, "Position", "Width", Quantities::ratio, Quantities::length);
    }

    return nullptr;
}

StringVM* MainVM::getStringVM() {
    if(bow.has_value()) {
        return new StringVM(this, bow->string);
    }

    return nullptr;
}

MassesVM* MainVM::getMassesVM() {
    if(bow.has_value()) {
        return new MassesVM(this, bow->masses);
    }

    return nullptr;
}

DampingVM* MainVM::getDampingVM() {
    if(bow.has_value()) {
        return new DampingVM(this, bow->damping);
    }

    return nullptr;
}

LineVM* MainVM::getLineVM(int index) {
    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Line>(bow->profile.segments[index])) {
        return new LineVM(this, std::get<Line>(bow->profile.segments[index]));
    }

    return nullptr;
}

ArcVM* MainVM::getArcVM(int index) {
    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Arc>(bow->profile.segments[index])) {
        return new ArcVM(this, std::get<Arc>(bow->profile.segments[index]));
    }

    return nullptr;
}

SpiralVM* MainVM::getSpiralVM(int index) {
    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Spiral>(bow->profile.segments[index])) {
        return new SpiralVM(this, std::get<Spiral>(bow->profile.segments[index]));
    }

    return nullptr;
}

TableModel* MainVM::getSplineVM(int index) {
    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Spline>(bow->profile.segments[index])) {
        return new TableModel(this, std::get<Spline>(bow->profile.segments[index]).points, "X", "Y", Quantities::length, Quantities::length);
    }

    return nullptr;
}

void MainVM::newFile() {
    // Create  default bow data, which is not linked to any file yet
    this->bow = new_model();
    this->modelTreeSelectionVM->clearSelection();    // Needs to be done before resetting the model in order to get a selection changed signal
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
    this->modelTreeSelectionVM->clearSelection();    // Needs to be done before resetting the model in order to get a selection changed signal
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
