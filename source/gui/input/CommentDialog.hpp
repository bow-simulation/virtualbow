#pragma once
#include <QtWidgets>
#include "gui/PersistentDialog.hpp"

class InputData;

class CommentDialog: public PersistentDialog
{
public:
    CommentDialog(QWidget* parent, InputData& data);

private:
    InputData& data;
    QTextEdit* edit;
};
