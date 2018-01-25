#pragma once
#include "gui/input/editors/TreeEditor.hpp"
#include "gui/input/views/LimbView.hpp"

class InputData;

class BowEditor: public QSplitter
{
    Q_OBJECT

public:
    BowEditor();
    const InputData& getData() const;
    void setData(const InputData& data);

signals:
    void modified();

private:
    TreeEditor* edit;
    LimbView* view;
};
