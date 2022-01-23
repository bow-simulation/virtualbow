#pragma once
#include <QPlainTextEdit>

class DataViewModel;

class CommentView: public QPlainTextEdit
{
public:
    CommentView(DataViewModel* model);

private:
    DataViewModel* model;

    void updateModel(QObject* source);
    void updateView(QObject* source);
};
