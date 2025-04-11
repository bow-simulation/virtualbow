#pragma once
#include <QWidget>

class CommentsVM;

class CommentsView: public QWidget
{
public:
    CommentsView(CommentsVM* model);
};
