#pragma once
#include <QObject>

class DataViewModel;

class MainViewModel: public QObject
{
    Q_OBJECT

public:
    MainViewModel();

    DataViewModel* dataModel();
    QString filePath() const;
    QString displayPath() const;
    bool isModified() const;

    void loadDefaults();
    void loadFile(const QString& path);
    void saveFile(const QString& path);

signals:
    void displayPathChanged(const QString& path);
    void modificationChanged(bool modified);


private:
    DataViewModel* data_model;
    QString file_path;
    bool modified;

    void setFilePath(const QString& value);
    void setModified(bool value);
};
