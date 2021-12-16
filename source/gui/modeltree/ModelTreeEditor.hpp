#pragma once
#include "solver/model/input/InputData.hpp"
#include <QTreeWidget>

class ModelTreeEditor: public QTreeWidget {
    Q_OBJECT

public:
    ModelTreeEditor();
    const InputData& getData() const;
    void setData(const InputData& data);

signals:
    void modified();

private:
    InputData data;
};
