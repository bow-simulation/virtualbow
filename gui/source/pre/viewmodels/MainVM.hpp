#pragma once
#include "solver/BowModel.hpp"
#include <QObject>
#include <QProperty>
#include <QTimer>
#include <optional>

// Root view model of the application
// Manages loading/saving/exporting files and the associated logic like keeping track of unsaved changes.
// Also provides access to the various other sub-viewmodels.

class ModelTreeVM;
class CommentsVM;
class SettingsVM;
class DimensionsVM;
class MaterialVM;
class LayerVM;
class StringVM;
class MassesVM;
class DampingVM;
class LineVM;
class ArcVM;
class SpiralVM;
class SplineVM;
class TableModel;
class QItemSelectionModel;
class QAbstractItemModel;

class MainVM: public QObject {
    Q_OBJECT

public:
    MainVM();

    // Submodels

    ModelTreeVM* getModelTreeVM();
    QItemSelectionModel* getModelTreeSelectionVM();

    CommentsVM* getCommentsVM();
    SettingsVM* getSettingsVM();
    DimensionsVM* getDimensionsVM();
    MaterialVM* getMaterialVM(int index);
    LayerVM* getLayerVM(int index);
    TableModel* getLayerHeightVM(int index);
    TableModel* getWidthVM();
    StringVM* getStringVM();
    MassesVM* getMassesVM();
    DampingVM* getDampingVM();

    LineVM* getLineVM(int index);        // Returns line viewmodel for given segment index or nullptr if the segment is of another type
    ArcVM* getArcVM(int index);          // Returns arc viewmodel for given segment index or nullptr if the segment is of another type
    SpiralVM* getSpiralVM(int index);    // Returns spiral viewmodel for given segment index or nullptr if the segment is of another type
    TableModel* getSplineVM(int index);  // Returns spline viewmodel for given segment index or nullptr if the segment is of another type

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
    ModelTreeVM* modelTreeVM;
    QItemSelectionModel* modelTreeSelectionVM;
};
