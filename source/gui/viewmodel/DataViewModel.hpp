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

    // Comments

    QString get_comments() const;
    void set_comments(const QString& value);

    // Settings

    int get_limb_elements() const;
    void set_limb_elements(int value);

    int get_string_elements() const;
    void set_string_elements(int value);

    int get_draw_steps() const;
    void set_draw_steps(int value);

    double get_arrow_clamp_force() const;
    void set_arrow_clamp_force(double value);

    double get_time_span_factor() const;
    void set_time_span_factor(double value);

    double get_time_step_factor() const;
    void set_time_step_factor(double value);

    double get_sampling_rate() const;
    void set_sampling_rate(double value);


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
    void anyModified();
    void allModified();

    void commentModified();
    void settingsModified();
    void dimensionsModified();

private:
    InputData data;
};
