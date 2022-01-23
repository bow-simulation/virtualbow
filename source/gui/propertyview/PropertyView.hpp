#pragma once
#include <QDockWidget>
#include <functional>

class DataViewModel;
class CommentView;
class QStackedWidget;

class QLabel;  // TODO: Remove

class PropertyView: public QDockWidget {
public:
    PropertyView(DataViewModel* model);

private:
    DataViewModel* model;
    QStackedWidget* stack;

    QWidget* placeholder = nullptr;
    QWidget* comments = nullptr;
    QWidget* settings = nullptr;
    QWidget* dimensions = nullptr;
    // ...
    QWidget* masses = nullptr;

    void showPlaceholder();
    void showComments();
    void showSettings();
    void showDimensions();
    // ...
    void showMasses();


    void showWidget(const QString& title, QWidget* widget, const std::function<QWidget*(DataViewModel*)>& create_widget);
};
