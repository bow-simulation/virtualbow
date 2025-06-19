#pragma once
#include "solver/BowModel.hpp"
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
class DimensionsModel;
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

    // Submodels

    MainTreeModel* getMainTreeModel();
    QItemSelectionModel* getModelTreeSelectionModel();

    CommentsModel* getCommentsModel();
    SettingsModel* getSettingsModel();
    DimensionsModel* getDimensionsModel();
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

    // Current state

    const QString& currentFile() const;    // Currently loaded file path or empty, if no file is currently loaded
    bool hasBowModel() const;              // Whether bow model data is currently present (does not have to be backed by a file)
    bool hasUnsavedWork() const;           // Whether model content has been modified without saving

signals:
    void currentFileChanged(const QString&);
    void hasBowModelChanged(bool);
    void hasUnsavedWorkChanged(bool);

    void contentModified();

private:
    // Top level state
    std::optional<BowModel> bow;    // Bow data, which might or might not be present
    QString path;                   // Path of the currently loaded file (or empty if no file is loaded)
    bool unsaved;                  // Whether there are any unsaved modifications to the bow model


    // Sub Viewmodels
    MainTreeModel* mainTreeModel;
    QItemSelectionModel* modelTreeSelectionModel;
};
