#include "PropertyView.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeView.hpp"
#include "gui/widgets/propertytree/PropertyTreeModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeItem.hpp"
#include "CommentView.hpp"
#include <QStackedWidget>
#include <QLabel>

PropertyView::PropertyView(DataViewModel* model)
    : model(model),
      stack(new QStackedWidget())
{
    this->setObjectName("PropertyView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWidget(stack);    

    QObject::connect(model, &DataViewModel::noneSelected, this, &PropertyView::showPlaceholder);
    QObject::connect(model, &DataViewModel::commentsSelected, this, &PropertyView::showComments);
    QObject::connect(model, &DataViewModel::settingsSelected, this, &PropertyView::showSettings);
    QObject::connect(model, &DataViewModel::dimensionsSelected, this, &PropertyView::showDimensions);
    // ...
    QObject::connect(model, &DataViewModel::massesSelected, this, &PropertyView::showMasses);

    showPlaceholder();

    /*
    QObject::connect(model, &DataViewModel::nothingSelected, this, [&, model, stack]{
        stack->setCurrentWidget(placeholder);
        this->setWindowTitle(DEFAULT_TILE);
    });

    QObject::connect(model, &DataViewModel::commentsSelected, this, [&, model, stack]{
        if(comment_view == nullptr) {
            comment_view = new CommentView(model);
            stack->addWidget(comment_view);
        }

        stack->setCurrentWidget(comment_view);
        this->setWindowTitle("Comments");
    });
    */

    /*
    QObject::connect(model, &DataViewModel::settingsSelected, this, [&]{
        if(settings_view == nullptr) {
            settings_view = new QLabel("Settings");
            stack->addWidget(settings_view);
        }
        stack->setCurrentWidget(settings_view);
    });

    QObject::connect(model, &DataViewModel::dimensionsSelected, this, [&]{
        if(dimensions_view == nullptr) {
            dimensions_view = new QLabel("Dimensions");
            stack->addWidget(dimensions_view);
        }
        stack->setCurrentWidget(dimensions_view);
    });
    */
}

void PropertyView::showPlaceholder() {
    showWidget("Properties", placeholder, [](DataViewModel* model){
        return new QWidget();
    });
}

void PropertyView::showComments() {
    showWidget("Comments", comments, [](DataViewModel* model){
        return new CommentView(model);
    });
}

#include <QTreeWidget>
#include <QHeaderView>

// QTreePropertyBrowser
// https://github.com/qtproject/qt-solutions/tree/master/qtpropertybrowser

void PropertyView::showSettings() {
    showWidget("Settings", settings, [&](DataViewModel* model) {

        auto root = new PropertyTreeItem();

        auto item_general = new PropertyTreeItem("General", QVariant(), root);
        auto item_statics = new PropertyTreeItem("Statics", QVariant(), root);
        auto item_dynamics = new PropertyTreeItem("Dynamics", QVariant(), root);

        new PropertyTreeItem("Limb elements", 25, item_general);
        new PropertyTreeItem("String elements", 25, item_general);

        new PropertyTreeItem("Draw steps", 150, item_statics);

        new PropertyTreeItem("Arrow clamp force", 0.0, item_dynamics);
        new PropertyTreeItem("Time span factor", 1.0, item_dynamics);
        new PropertyTreeItem("Time step factor", 1.5, item_dynamics);
        new PropertyTreeItem("Sampling rate", 500.0, item_dynamics);

        auto tree = new PropertyTreeView(new PropertyTreeModel(root, this));
        return tree;
    });
}

#include <QTableWidget>
#include <QTableWidgetItem>

void PropertyView::showDimensions() {
    showWidget("Dimensions", dimensions, [](DataViewModel* model){
        auto table = new QTableWidget(7, 2);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->horizontalHeader()->hide();
        table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        table->verticalHeader()->setDefaultSectionSize(24);
        table->verticalHeader()->hide();

        QFont bold;
        bold.setBold(true);

        auto item1 = new QTableWidgetItem("Draw");
        item1->setFont(bold);

        table->setItem(0, 0, item1);
        table->setSpan(0, 0, 1, 2);

        table->setItem(1, 0, new QTableWidgetItem("Brace height"));
        table->setItem(1, 1, new QTableWidgetItem("0.2"));

        table->setItem(2, 0, new QTableWidgetItem("Draw length"));
        table->setItem(2, 1, new QTableWidgetItem("0.7"));

        auto item2 = new QTableWidgetItem("Handle");
        item2->setFont(bold);

        table->setItem(3, 0, item2);
        table->setSpan(3, 0, 1, 2);

        table->setItem(4, 0, new QTableWidgetItem("Length"));
        table->setItem(4, 1, new QTableWidgetItem("0.0"));

        table->setItem(5, 0, new QTableWidgetItem("Setback"));
        table->setItem(5, 1, new QTableWidgetItem("0.0"));

        table->setItem(6, 0, new QTableWidgetItem("Angle"));
        table->setItem(6, 1, new QTableWidgetItem("0.0"));

        return table;
    });
}

void PropertyView::showMasses() {
    showWidget("Masses", masses, [&](DataViewModel* model) {
        return new QLabel("Hello");
    });
}

void PropertyView::showWidget(const QString& title, QWidget* widget, const std::function<QWidget*(DataViewModel*)>& create_widget) {
    if(widget == nullptr) {
        widget = create_widget(model);
        stack->addWidget(widget);
    }

    stack->setCurrentWidget(widget);
    this->setWindowTitle(title);
}
