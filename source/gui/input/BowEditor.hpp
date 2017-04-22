#pragma once
#include <QtWidgets>

class InputData;

class BowEditor: public QSplitter
{
public:
    BowEditor(InputData& data);
};
