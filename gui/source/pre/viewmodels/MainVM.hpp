#pragma once
#include <QObject>
#include <QProperty>
#include <QTimer>

// Root view model of the application
// Manages loading/saving/exporting files and the associated logic like keeping track of unsaved changes.
// Also provides access to the various other sub-viewmodels.

class ModelTreeVM;

class MainVM: public QObject {
    Q_OBJECT
    //Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)
    //Q_PROPERTY(bool unsavedWork READ hasUnsavedWork NOTIFY unsavedWorkChanged)

public:
    MainVM();

    // Submodels

    ModelTreeVM* getModelTreeVM();

    // File actions

    void newFile();                            // Creates a new default model that isn't connected to a file yet
    void loadFile(const QString& path);    // Loads a bow model from a file path
    void saveFile(const QString& path);    // Saves the bow model to a file path

    // Current state

    const QString& currentFile() const;    // Currently loaded file path or empty, if no file is currently loaded
    bool hasUnsavedWork() const;           // Whether model content has been modified without saving

signals:
    void currentFileChanged(const QString&);
    void unsavedWorkChanged(bool);

private:
    // Top level state
    QString path;        // Path of the currently loaded file (or empty if no file is loaded)
    bool unsaved;        // Whether there are any unsaved modifications to the bow model

    // Sub Viewmodels
    ModelTreeVM* modelTreeVM;
};
