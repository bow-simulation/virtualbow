#pragma once
#include "gui/input/CommentsDialog.hpp"
#include "gui/input/SettingsDialog.hpp"
#include <QtWidgets>
#include <functional>

class TreeItem: public QTreeWidgetItem
{
public:
    std::function<void()> action;

    template<class parent_t>
    TreeItem(parent_t* parent, const QString& name, const QIcon& icon, const std::function<void()>& action = [](){})
        : QTreeWidgetItem(parent, {name}),
          action(action)
    {
        this->setIcon(0, icon);
    }
};


class ModelTree: public QTreeWidget
{
public:
    ModelTree(InputData& data)
    {
        new TreeItem(this, "Comments", QIcon(":/icons/comments"), [&](){ CommentsDialog(this, data); });
        new TreeItem(this, "Settings", QIcon(":/icons/settings"), [&](){ SettingsDialog(this, data); });
        auto item_parameters = new TreeItem(this, "Parameters", QIcon(":/icons/circle-grey"));

        auto item_limbs = new TreeItem(item_parameters, "Limbs", QIcon(":/icons/circle-grey"));
        new TreeItem(item_limbs, "Material", QIcon(":/icons/circle-grey"));
        new TreeItem(item_limbs, "Profile", QIcon(":/icons/circle-grey"));
        new TreeItem(item_limbs, "Width", QIcon(":/icons/circle-grey"));
        new TreeItem(item_limbs, "Height", QIcon(":/icons/circle-grey"));

        new TreeItem(item_parameters, "String", QIcon(":/icons/circle-grey"));
        new TreeItem(item_parameters, "Masses", QIcon(":/icons/circle-grey"));
        new TreeItem(item_parameters, "Operation", QIcon(":/icons/circle-grey"));

        QObject::connect(this, &QTreeWidget::itemActivated, [](QTreeWidgetItem* item, int column)
        {
            auto ptr = dynamic_cast<TreeItem*>(item);
            if(ptr)
            {
                ptr->action();
            }
        });

        this->setHeaderLabel("Model Tree");
        this->expandAll();
    }

};
