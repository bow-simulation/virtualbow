#pragma once
#include "solver/model/input/InputData.hpp"
#include <QSplitter>

class ModelTreeEditor;
class LimbView;

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
