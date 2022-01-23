#pragma once
#include <QStackedWidget>

class DataViewModel;
class CommentView;

class QLabel;  // TODO: Remove

class PropertyView: public QStackedWidget {
public:
    PropertyView(DataViewModel* model);

private:
    QWidget* placeholder = new QWidget();

    CommentView* comment_view = nullptr;
    QLabel* settings_view = nullptr;
    QLabel* dimensions_view = nullptr;
};
