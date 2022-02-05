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

    Material& addMaterial(int index);
    void removeMaterial(int index);
    void swapMaterials(int i, int j);

    Layer& addLayer(int index);
    void removeLayer(int index);
    void swapLayers(int i, int j);

signals:
    void anyModified(QObject* source);
    void allModified(QObject* source);

    void commentModified(QObject* source);
    void settingsModified(QObject* source);
    void dimensionsModified(QObject* source);

private:
    InputData data;
};
