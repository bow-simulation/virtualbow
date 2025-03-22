#pragma once
#include <QObject>
#include <QProperty>
#include <QTimer>

// Root view model of the application
// Manages loading/saving/exporting files and the associated logic like keeping track of unsaved changes.
// Also provides access to the various other sub-viewmodels.

class MainVM: public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)
    Q_PROPERTY(bool unsavedWork READ hasUnsavedWork NOTIFY unsavedWorkChanged)

public:
    MainVM():
        filePath(""),
        unsaved(false)
    {

    }

    // File actions

    void newFile() {
        this->filePath = "";
        this->unsaved = false;

        emit currentFileChanged(filePath);
        emit unsavedWorkChanged(unsaved);
    }

    void loadFile(const QString& filePath) {
        this->filePath = filePath;
        this->unsaved = false;

        emit currentFileChanged(filePath);
        emit unsavedWorkChanged(unsaved);
    }

    void saveFile(const QString& filePath) {
        this->filePath = filePath;
        this->unsaved = false;

        emit currentFileChanged(filePath);
        emit unsavedWorkChanged(unsaved);
    }

    // Current state

    const QString& currentFile() const {
        return filePath;
    }

    bool hasUnsavedWork() const {
        return unsaved;
    }

signals:
    void currentFileChanged(const QString&);
    void unsavedWorkChanged(bool);

private:
    QString filePath;    // Path of the currently loaded file (or empty if no file is loaded)
    bool unsaved;        // Whether there are any unsaved modifications to the bow model
};

/*
class MainVM: public QObject {
public:
    MainVM():
        file("Initial"),
        unsaved(false)
    {

    }

    QString currentFile() const {
        return file;
    }

    QProperty<QString>& currentFileProperty() {
        return file;
    }

    bool hasUnsavedWork() const {
        return unsaved;
    }

    QProperty<bool>& unsavedWorkProperty() {
        return unsaved;
    }

    void modify() {
        unsaved.setValue(!unsaved.value());
        if(unsaved){
            file = "Unsaved";
        }
        else {
            file = "Saved";
        }
    }

private:
    QProperty<QString> file;
    QProperty<bool> unsaved;
};
*/
