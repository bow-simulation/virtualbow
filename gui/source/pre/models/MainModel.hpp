#pragma once
#include "solver/BowModel.hpp"
#include "solver/BowResult.hpp"
#include <QObject>
#include <QProperty>
#include <QTimer>
#include <optional>

// Root view model of the application
// Manages loading/saving/exporting files and the associated logic like keeping track of unsaved changes.
// Also provides access to the various other sub-models.

class MainTreeModel;
class CommentsModel;
class SettingsModel;
class HandleModel;
class DrawModel;
class MaterialModel;
class LayerModel;
class StringModel;
class MassesModel;
class DampingModel;
class LineModel;
class ArcModel;
class SpiralModel;
class SplineModel;
class TableModel;
class QItemSelectionModel;
class QAbstractItemModel;

class MainModel: public QObject {
    Q_OBJECT

public:
    MainModel();

    // Current state

    const QString& currentFile() const;    // Currently loaded file path or empty, if no file is currently loaded
    bool hasCurrentFile() const;           // Whether the bow model is associated with a file
    bool hasUnsavedWork() const;           // Whether model content has been modified without saving
    bool hasBow() const;              // Whether bow model data is currently present (does not have to be backed by a file)
    const BowModel& getBow() const;

    bool hasGeometry() const;
    const LimbInfo& getGeometry() const;

    bool hasError() const;
    const QString& getError() const;

    // Submodels

    MainTreeModel* getMainTreeModel();
    QItemSelectionModel* getModelTreeSelectionModel();

    CommentsModel* getCommentsModel();
    SettingsModel* getSettingsModel();
    HandleModel* getHandleModel();
    DrawModel* getDrawModel();
    MaterialModel* getMaterialModel(int index);
    LayerModel* getLayerModel(int index);
    TableModel* getLayerHeightModel(int index);
    TableModel* getWidthModel();
    StringModel* getStringModel();
    MassesModel* getMassesModel();
    DampingModel* getDampingModel();

    LineModel* getLineModel(int index);        // Returns line models for given segment index or nullptr if the segment is of another type
    ArcModel* getArcModel(int index);          // Returns arc models for given segment index or nullptr if the segment is of another type
    SpiralModel* getSpiralModel(int index);    // Returns spiral models for given segment index or nullptr if the segment is of another type
    TableModel* getSplineModel(int index);  // Returns spline models for given segment index or nullptr if the segment is of another type

    // File actions

    void newFile();                        // Creates a new default model that isn't connected to a file yet
    void loadFile(const QString& path);    // Loads a bow model from a file path
    void saveFile(const QString& path);    // Saves the bow model to a file path

signals:
    void currentFileChanged(const QString&);    // Emitted when the loaded file has changed
    void hasBowModelChanged(bool);              // Emitted when the property hasBowModel() has changed
    void hasUnsavedWorkChanged(bool);           // Emitted when the property hasUnsavedWork() has changed

    void contentModified();                     // Emitted when any content of the model has been modified
    void geometryChanged();                     // Emitted when the model geometry has been recomputed due to modifications

private:
    // Top level state
    QString path;                                    // Path of the currently loaded file (or empty if no file is loaded)
    bool converted;                                  // Whether the loaded file had to be converted to the current model format
    bool unsaved;                                    // Whether there are any unsaved modifications to the bow model

    std::optional<BowModel> bow;                     // Bow data, which might not be present
    std::optional<LimbInfo> geometry;    // Geometry of the bow, which might not be present
    std::optional<QString> error;                    // Possible error string from computing the geometry

    // Sub Viewmodels
    MainTreeModel* mainTreeModel;
    QItemSelectionModel* modelTreeSelectionModel;

    // (Re)Computes the bow's geometry and emots geometryChanged signal when finished
    void updateBowGeometry();

    // Connects a submodel to the main model. Requires the submodel to define a "contentModified" signal.
    template<typename SubModel>
    void connectSubModel(SubModel* subModel) {
        QObject::connect(subModel, &SubModel::contentModified, this, &MainModel::contentModified);
    }
};
