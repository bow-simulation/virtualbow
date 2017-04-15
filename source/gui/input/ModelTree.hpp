#pragma once
#include <QtWidgets>

class TreeItem: public QTreeWidgetItem
{
public:
    TreeItem(const QIcon& icon, const QString& name)
        : QTreeWidgetItem({name})
    {
        this->setIcon(0, icon);
    }

    TreeItem(QTreeWidgetItem* parent, const QIcon& icon, const QString& name)
        : TreeItem(icon, name)
    {
        parent->addChild(this);
    }
};


class ModelTree: public QTreeWidget
{
public:
    ModelTree()
    {
        auto item_comments = new TreeItem(QIcon(":/icons/comments"), "Comments");
        auto item_settings = new TreeItem(QIcon(":/icons/settings"), "Settings");
        auto item_parameters = new TreeItem(QIcon(":/icons/circle-grey"), "Parameters");

        auto item_limbs = new TreeItem(item_parameters, QIcon(":/icons/circle-grey"), "Limbs");
        new TreeItem(item_limbs, QIcon(":/icons/circle-grey"), "Material");
        new TreeItem(item_limbs, QIcon(":/icons/circle-grey"), "Profile");
        new TreeItem(item_limbs, QIcon(":/icons/circle-grey"), "Width");
        new TreeItem(item_limbs, QIcon(":/icons/circle-grey"), "Height");

        new TreeItem(item_parameters, QIcon(":/icons/circle-grey"), "String");
        new TreeItem(item_parameters, QIcon(":/icons/circle-grey"), "Masses");
        new TreeItem(item_parameters, QIcon(":/icons/circle-grey"), "Operation");

        this->addTopLevelItem(item_comments);
        this->addTopLevelItem(item_settings);
        this->addTopLevelItem(item_parameters);
        this->setHeaderLabel("Model Tree");
        this->expandAll();
    }

};
