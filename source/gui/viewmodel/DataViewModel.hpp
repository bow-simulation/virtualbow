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

    Material& createMaterial(int index, const QString& name);
    void removeMaterial(int index);
    void swapMaterials(int i, int j);

    Layer& createLayer(int index, const QString& name);
    void removeLayer(int index);
    void swapLayers(int i, int j);

    void addSegment(int index, const SegmentInput& segment);
    void removeSegment(int index);
    void swapSegments(int i, int j);

signals:
    void anyModified(QObject* source);
    void allModified(QObject* source);

    void commentModified(QObject* source);
    void settingsModified(QObject* source);
    void dimensionsModified(QObject* source);

private:
    InputData data;
};
