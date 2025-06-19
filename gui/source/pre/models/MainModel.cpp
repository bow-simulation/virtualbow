#include "MainModel.hpp"
#include "MainTreeModel.hpp"
#include "CommentsModel.hpp"
#include "SettingsModel.hpp"
#include "DimensionsModel.hpp"
#include "MaterialModel.hpp"
#include "LayerModel.hpp"
#include "StringModel.hpp"
#include "MassesModel.hpp"
#include "DampingModel.hpp"
#include "LineModel.hpp"
#include "ArcModel.hpp"
#include "SpiralModel.hpp"
#include "pre/models/TableModel.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "solver/API.hpp"
#include <QItemSelectionModel>
#include <QAbstractItemModelTester>

MainModel::MainModel():
    bow(std::nullopt),
    path(""),
    unsaved(false),
    mainTreeModel(new MainTreeModel(this)),
    modelTreeSelectionModel(new QItemSelectionModel(mainTreeModel))
{
    // TODO: Move this to a dedicated test
    // new QAbstractItemModelTester(mainTreeModel, QAbstractItemModelTester::FailureReportingMode::Warning, this);

    //connectSubModel(mainTreeModel);

    QObject::connect(this, &MainModel::contentModified, this, [&]{
        if(!unsaved) {
            unsaved = true;
            emit hasUnsavedWorkChanged(true);
        }
    });
}

MainTreeModel* MainModel::getMainTreeModel() {
    return mainTreeModel;
}

QItemSelectionModel* MainModel::getModelTreeSelectionModel() {
    return modelTreeSelectionModel;
}

CommentsModel* MainModel::getCommentsModel() {
    if(bow.has_value()) {
        return new CommentsModel(this, bow->comment);
    }

    return nullptr;
}

SettingsModel* MainModel::getSettingsModel() {
    if(bow.has_value()) {
        return new SettingsModel(this, bow->settings);
    }

    return nullptr;
}

DimensionsModel* MainModel::getDimensionsModel() {
    if(bow.has_value()) {
        return new DimensionsModel(this, bow->dimensions);
    }

    return nullptr;
}

MaterialModel* MainModel::getMaterialModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->materials.size()) {
        return new MaterialModel(this, bow->materials[index]);
    }

    return nullptr;
}

LayerModel* MainModel::getLayerModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->layers.size()) {
        return new LayerModel(this, bow->layers[index], bow->materials);
    }

    return nullptr;
}

TableModel* MainModel::getLayerHeightModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->layers.size()) {
        return new TableModel(this, bow->layers[index].height, "Position", "Height", Quantities::ratio, Quantities::length);
    }

    return nullptr;
}

TableModel* MainModel::getWidthModel() {
    if(bow.has_value()) {
        return new TableModel(this, bow->width, "Position", "Width", Quantities::ratio, Quantities::length);
    }

    return nullptr;
}

StringModel* MainModel::getStringModel() {
    if(bow.has_value()) {
        return new StringModel(this, bow->string);
    }

    return nullptr;
}

MassesModel* MainModel::getMassesModel() {
    if(bow.has_value()) {
        return new MassesModel(this, bow->masses);
    }

    return nullptr;
}

DampingModel* MainModel::getDampingModel() {
    if(bow.has_value()) {
        return new DampingModel(this, bow->damping);
    }

    return nullptr;
}

LineModel* MainModel::getLineModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Line>(bow->profile.segments[index])) {
        return new LineModel(this, std::get<Line>(bow->profile.segments[index]));
    }

    return nullptr;
}

ArcModel* MainModel::getArcModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Arc>(bow->profile.segments[index])) {
        return new ArcModel(this, std::get<Arc>(bow->profile.segments[index]));
    }

    return nullptr;
}

SpiralModel* MainModel::getSpiralModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Spiral>(bow->profile.segments[index])) {
        return new SpiralModel(this, std::get<Spiral>(bow->profile.segments[index]));
    }

    return nullptr;
}

TableModel* MainModel::getSplineModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Spline>(bow->profile.segments[index])) {
        return new TableModel(this, std::get<Spline>(bow->profile.segments[index]).points, "X", "Y", Quantities::length, Quantities::length);
    }

    return nullptr;
}

void MainModel::newFile() {
    // Create  default bow data, which is not linked to any file yet
    this->bow = new_model();
    this->modelTreeSelectionModel->clearSelection();    // Needs to be done before resetting the model in order to get a selection changed signal
    this->mainTreeModel->setBowModel(&bow.value());
    this->path = "";
    this->unsaved = false;

    emit currentFileChanged(path);
    emit hasBowModelChanged(true);
    emit hasUnsavedWorkChanged(unsaved);
}

void MainModel::loadFile(const QString& path) {
    // Load bow data from file, which becomes the current path.
    this->bow = load_model(path.toStdString(), true);
    this->path = path;
    this->unsaved = false;
    this->modelTreeSelectionModel->clearSelection();    // Needs to be done before resetting the model in order to get a selection changed signal
    this->mainTreeModel->setBowModel(&bow.value());

    emit currentFileChanged(path);
    emit hasBowModelChanged(true);    // Might not have actually changed, but doesn't really matter
    emit hasUnsavedWorkChanged(unsaved);
}

void MainModel::saveFile(const QString& path) {
    // If bow data is available, save it to the file, which becomes the current path.
    if(bow.has_value()) {
        save_model(bow.value(), path.toStdString());
        this->path = path;
        this->unsaved = false;

        emit currentFileChanged(path);
        emit hasUnsavedWorkChanged(unsaved);
    }
}

const QString& MainModel::currentFile() const {
    return path;
}

bool MainModel::hasBowModel() const {
    return bow.has_value();
}

bool MainModel::hasUnsavedWork() const {
    return unsaved;
}
