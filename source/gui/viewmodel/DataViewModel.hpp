#pragma once
#include <QObject>
#include "solver/model/input/InputData.hpp"

class DataViewModel: public QObject
{
    Q_OBJECT

public:
    DataViewModel();

    void loadDefaults();
    void loadFile(const QString& path);
    void saveFile(const QString& path);

    const InputData& getData() const;
    InputData& getData();

signals:
    void noneSelected();
    void commentsSelected();
    void settingsSelected();
    void dimensionsSelected();
    void materialsSelected();
    void layersSelected();
    void profileSelected();
    void widthSelected();
    void stringSelected();
    void massesSelected();
    void dampingSelected();

    void anyModified(QObject* source);
    void allModified(QObject* source);

    void commentModified(QObject* source);
    void settingsModified(QObject* source);
    void dimensionsModified(QObject* source);

private:
    InputData data;
};
