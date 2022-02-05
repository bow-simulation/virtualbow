#include "PropertyView.hpp"
#include <QStackedWidget>
#include <QLabel>

PropertyView::PropertyView()
    : stack(new QStackedWidget()),
      placeholder(new QLabel("Placeholder"))
{
    this->setWindowTitle("Properties");
    this->setObjectName("PropertyView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWidget(placeholder);
}

void PropertyView::showEditor(QWidget* editor) {
    if(editor == nullptr) {
        this->setWidget(placeholder);
    }
    else {
        this->setWidget(editor);
    }
}

/*
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

#include <QDebug>
#include <QTreeWidget>

void PropertyView::showSettings() {
    showWidget("Settings", settings, [&](DataViewModel* model) {
        return new QLabel("Settings");
    });
}

#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/GroupTreeItem.hpp"
#include "gui/widgets/propertytree/items/DoubleTreeItem.hpp"
#include "gui/widgets/propertytree/items/IntegerTreeItem.hpp"
#include "gui/widgets/propertytree/items/StringTreeItem.hpp"
#include "gui/widgets/propertytree/items/ColorTreeItem.hpp"

void PropertyView::showDimensions() {
    showWidget("Dimensions", dimensions, [](DataViewModel* model){

        auto item_group1 = new GroupTreeItem("Group 1");
        auto item_group2 = new GroupTreeItem("Group 2");
        auto item_group3 = new GroupTreeItem("Group 3");

        auto item_integer = new IntegerTreeItem2("Integer", item_group1);
        auto item_double = new DoubleTreeItem("Double", item_group1);
        auto item_string = new StringTreeItem("String", item_group1);
        auto item_color = new ColorTreeItem("Color", item_group1);

        auto tree = new PropertyTreeWidget();
        tree->addTopLevelItem(item_group1);
        tree->addTopLevelItem(item_group2);
        tree->addTopLevelItem(item_group3);
        tree->expandAll();

        return tree;
    });
}

void PropertyView::showMasses() {
    showWidget("Masses", masses, [&](DataViewModel* model) {
        return new QLabel("Masses");
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
*/
