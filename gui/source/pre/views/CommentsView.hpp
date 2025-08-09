#pragma once
#include <QWidget>

class CommentsModel;

class CommentsView: public QWidget
{
public:
    CommentsView(CommentsModel* model);
};
