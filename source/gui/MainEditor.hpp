#pragma once
#include "gui/modeltree/ModelTreeEditor.hpp"
#include "gui/limbview/LimbView.hpp"
#include "gui/units/UnitSystem.hpp"

class InputData;

class MainEditor: public QSplitter {
    Q_OBJECT

public:
    MainEditor();
    InputData getData() const;
    void setData(const InputData& data);

signals:
    void modified();

private:
    ModelTreeEditor* edit;
    LimbView* view;
};
