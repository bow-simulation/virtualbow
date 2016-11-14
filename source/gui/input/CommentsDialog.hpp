#pragma once
#include <QtWidgets>

class InputData;

class CommentsDialog: public QDialog
{
public:
    CommentsDialog(QWidget* parent, InputData& data);

private:
    InputData& data;
    QTextEdit* edit;
};
