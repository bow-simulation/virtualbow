#pragma once
#include "bow/input2/InputData.hpp"
#include "gui/input2/dialogs/CommentDialog.hpp"
#include "gui/input2/dialogs/ProfileDialog.hpp"
#include "gui/input2/dialogs/WidthDialog.hpp"
#include "gui/input2/dialogs/LayerDialog.hpp"
#include "gui/input2/dialogs/SettingsDialog.hpp"
#include "gui/input2/dialogs/StringDialog.hpp"
#include "gui/input2/dialogs/MassesDialog.hpp"
#include "gui/input2/dialogs/OperationDialog.hpp"
#include "gui/input2/GroupDialog.hpp"
#include "gui/input2/IntegerEditor.hpp"

#include <QtWidgets>
#include <functional>

class TreeItem: public QObject, public QTreeWidgetItem
{
    Q_OBJECT

public:
    template<class parent_t>
    TreeItem(parent_t* parent, const QString& name, const QIcon& icon, std::function<void()> action)
        : QTreeWidgetItem(parent, {name}),
          action(action)
    {
        this->setIcon(0, icon);
    }

    void performAction()
    {
        action();
    }

private:
    std::function<void()> action;
};

class ModelTree: public QTreeWidget
{
public:
    ModelTree()
    {
        new TreeItem(this, "Comments", QIcon(":/icons/model-tree/comments"), [&]{
            if(comments == nullptr)
                comments = new CommentDialog(this);

            comments->setData(input.meta.comments);
            comments->activateWindow();
            comments->show();

            /*
            QObject::connect(comments, &LayerDialog::accepted, [&]{
                input.meta.comments = comments->getData();
            });
            */
        });

        new TreeItem(this, "Settings", QIcon(":/icons/model-tree/settings"), [&]{
            auto dialog = new SettingsDialog(this);
            dialog->setData(input.settings);
            dialog->exec();
        });

        auto item_parameters = new QTreeWidgetItem(this, {"Parameters"});
        item_parameters->setIcon(0, QIcon(":/icons/model-tree/parameters"));

        new TreeItem(item_parameters, "Profile", QIcon(":/icons/model-tree/profile"), [&]
        {
            auto dialog = new ProfileDialog(this);
            dialog->setData(input.profile);
            dialog->exec();
        });

        new TreeItem(item_parameters, "Width", QIcon(":/icons/model-tree/width"), [&]
        {
            auto dialog = new WidthDialog(this);
            dialog->setData(input.width);
            dialog->exec();
        });

        new TreeItem(item_parameters, "Layers", QIcon(":/icons/model-tree/layers"), [&]{
            if(layers == nullptr)
                layers = new LayerDialog(this);

            layers->setData(input.layers);
            layers->activateWindow();
            layers->show();

            /*
            QObject::connect(layers, &LayerDialog::accepted, [&]{
                input.layers = layers->getData();
            });
            */
        });

        new TreeItem(item_parameters, "String", QIcon(":/icons/model-tree/string"), [&]
        {
            auto dialog = new StringDialog(this);
            dialog->setData(input.string);
            dialog->exec();
        });

        new TreeItem(item_parameters, "Masses", QIcon(":/icons/model-tree/masses"), [&]
        {
            auto dialog = new MassesDialog(this);
            dialog->setData(input.masses);
            dialog->exec();
        });

        new TreeItem(item_parameters, "Operation", QIcon(":/icons/model-tree/operation"), [&]
        {
            auto dialog = new OperationDialog(this);
            dialog->setData(input.operation);
            dialog->exec();
        });

        QObject::connect(this, &QTreeWidget::itemActivated, [](QTreeWidgetItem* base_item, int column)
        {
            auto item = dynamic_cast<TreeItem*>(base_item);
            if(item)
            {
                item->performAction();
            }
        });

        this->setHeaderLabel("Model Tree");
        this->expandAll();
        this->setItemsExpandable(false);    // Todo: Why is the expansion symbol still visible? (on KDE Desktop at least)
    }

private:
    InputData2 input;

    LayerDialog* layers = nullptr;
    CommentDialog* comments = nullptr;
};
