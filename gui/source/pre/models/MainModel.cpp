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
#include <QTimer>

// Delay time before recomputing the bow's geometry after changes to the model were made
// This avoids frequent recomputations when many changes happen quickly (e.g. when scrolling in a spinbox)
const int GEOMETRY_UPDATE_DELAY_MS = 100;

MainModel::MainModel():
    bow(std::nullopt),
    path(""),
    converted(false),
    unsaved(false),
    mainTreeModel(new MainTreeModel()),
    modelTreeSelectionModel(new QItemSelectionModel(mainTreeModel))
{
    connectSubModel(mainTreeModel);

    // Track changes to unsaved work by reacting to content modifications
    QObject::connect(this, &MainModel::contentModified, this, [&]{
        if(!unsaved) {
            unsaved = true;
            emit hasUnsavedWorkChanged(true);
        }
    });

    // Timer for recomputation of the bow geometry
    auto timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(GEOMETRY_UPDATE_DELAY_MS);

    // When the content is modified, start the timer if not already running.
    // Ignore the additional modification events if already running.
    QObject::connect(this, &MainModel::contentModified, this, [timer]{
        if(!timer->isActive()) {
            timer->start();
        }
    });

    // When the timer is completed, recompute the bow geometry and emit change signal when done
    QObject::connect(timer, &QTimer::timeout, this, &MainModel::updateBowGeometry);
}

const QString& MainModel::currentFile() const {
    return path;
}

bool MainModel::hasCurrentFile() const {
    return !path.isEmpty();
}

bool MainModel::hasUnsavedWork() const {
    return unsaved;
}

bool MainModel::hasBow() const {
    return bow.has_value();
}

const BowModel& MainModel::getBow() const {
    return *bow;
}

bool MainModel::hasGeometry() const {
    return geometry.has_value();
}

const LimbInfo& MainModel::getGeometry() const {
    return *geometry;
}

bool MainModel::hasError() const {
    return error.has_value();
}

const QString& MainModel::getError() const {
    return *error;
}

MainTreeModel* MainModel::getMainTreeModel() {
    return mainTreeModel;
}

QItemSelectionModel* MainModel::getModelTreeSelectionModel() {
    return modelTreeSelectionModel;
}

CommentsModel* MainModel::getCommentsModel() {
    if(bow.has_value()) {
        auto model = new CommentsModel(bow->comment);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

SettingsModel* MainModel::getSettingsModel() {
    if(bow.has_value()) {
        auto model = new SettingsModel(bow->settings);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

DimensionsModel* MainModel::getDimensionsModel() {
    if(bow.has_value()) {
        auto model = new DimensionsModel(bow->dimensions);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

MaterialModel* MainModel::getMaterialModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->section.materials.size()) {
        auto model = new MaterialModel(*std::next(bow->section.materials.begin(), index));
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

LayerModel* MainModel::getLayerModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->section.layers.size()) {
        auto model = new LayerModel(*std::next(bow->section.layers.begin(), index), bow->section.materials);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

TableModel* MainModel::getLayerHeightModel(int index) {
    if(bow.has_value() && index >= 0 && index < bow->section.layers.size()) {
        auto model = new TableModel(std::next(bow->section.layers.begin(), index)->height, "Position", "Height", Quantities::ratio, Quantities::length);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

TableModel* MainModel::getWidthModel() {
    if(bow.has_value()) {
        auto model = new TableModel(bow->section.width, "Position", "Width", Quantities::ratio, Quantities::length);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

StringModel* MainModel::getStringModel() {
    if(bow.has_value()) {
        auto model = new StringModel(bow->string);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

MassesModel* MainModel::getMassesModel() {
    if(bow.has_value()) {
        auto model = new MassesModel(bow->masses);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

DampingModel* MainModel::getDampingModel() {
    if(bow.has_value()) {
        auto model = new DampingModel(bow->damping);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

LineModel* MainModel::getLineModel(int index) {
    ProfileSegment& segment = *std::next(bow->profile.segments.begin(), index);

    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Line>(segment)) {
        auto model = new LineModel(std::get<Line>(segment));
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

ArcModel* MainModel::getArcModel(int index) {
    ProfileSegment& segment = *std::next(bow->profile.segments.begin(), index);

    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Arc>(segment)) {
        auto model = new ArcModel(std::get<Arc>(segment));
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

SpiralModel* MainModel::getSpiralModel(int index) {
    ProfileSegment& segment = *std::next(bow->profile.segments.begin(), index);

    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Spiral>(segment)) {
        auto model = new SpiralModel(std::get<Spiral>(segment));
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

TableModel* MainModel::getSplineModel(int index) {
    ProfileSegment& segment = *std::next(bow->profile.segments.begin(), index);

    if(bow.has_value() && index >= 0 && index < bow->profile.segments.size() && std::holds_alternative<Spline>(segment)) {
        auto model = new TableModel(std::get<Spline>(segment).points, "X", "Y", Quantities::length, Quantities::length);
        connectSubModel(model);
        return model;
    }

    return nullptr;
}

void MainModel::newFile() {
    // Create  default bow data, which is not linked to any file yet
    this->bow = new_model();
    this->modelTreeSelectionModel->clearSelection();    // Needs to be done before resetting the model in order to get a selection changed signal
    this->mainTreeModel->setBowModel(&bow.value());
    this->path = "";
    this->converted = false;
    this->unsaved = false;

    emit currentFileChanged(path);
    emit hasBowModelChanged(true);
    emit hasUnsavedWorkChanged(unsaved);

    updateBowGeometry();
}

void MainModel::loadFile(const QString& path) {
    // Load bow data from file, which becomes the current path.
    this->bow = load_model(path.toStdString(), this->converted);
    this->path = path;
    this->unsaved = false;
    this->modelTreeSelectionModel->clearSelection();    // Needs to be done before resetting the model in order to get a selection changed signal
    this->mainTreeModel->setBowModel(&bow.value());

    emit currentFileChanged(path);
    emit hasBowModelChanged(true);    // Might not have actually changed, but doesn't really matter
    emit hasUnsavedWorkChanged(unsaved);

    updateBowGeometry();
}

void MainModel::saveFile(const QString& path) {
    // If bow data is available, save it to the file, which becomes the current path.
    if(bow.has_value()) {
        save_model(bow.value(), path.toStdString(), this->converted);    // If the model data was converted from an older format, create a backup file when saving
        this->path = path;
        this->converted = false;
        this->unsaved = false;

        emit currentFileChanged(path);
        emit hasUnsavedWorkChanged(unsaved);
    }
}

void MainModel::updateBowGeometry() {
    if(bow.has_value()) {
        try {
            geometry = compute_geometry(*bow);
            error = std::nullopt;
        }
        catch(const std::exception& e) {
            geometry = std::nullopt;
            error = QString(e.what());
        }
    }
    else {
        geometry = std::nullopt;
        error = std::nullopt;
    }

    emit geometryChanged();
}
