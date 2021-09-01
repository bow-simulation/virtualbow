#pragma once
#include "gui/editors/TreeEditor.hpp"
#include "gui/views/LimbView.hpp"
#include "gui/units/UnitSystem.hpp"

class InputData;

class ModelEditor: public QSplitter {
    Q_OBJECT

public:
    ModelEditor();
    InputData getData() const;
    void setData(const InputData& data);

signals:
    void modified();

private:
    TreeEditor* edit;
    LimbView* view;
};
