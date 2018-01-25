#pragma once
#include "TreeEditor.hpp"

class Action
{
public:
    virtual void performAction() = 0;
};

class TreeEditor;

template<class DialogType, class DataType>
class TreeItem: public QTreeWidgetItem, public Action
{
public:
    TreeItem(TreeEditor* parent, DataType& data, const QString& name, const QIcon& icon)
        : QTreeWidgetItem(parent, {name}),
          parent(parent),
          dialog(nullptr),
          data(data)
    {
        this->setIcon(0, icon);
    }

    // Todo: Code duplication
    // Requirement: Item must have a TreeEditor as parent
    TreeItem(QTreeWidgetItem* parent, DataType& data, const QString& name, const QIcon& icon)
        : QTreeWidgetItem(parent, {name}),
          parent(static_cast<TreeEditor*>(parent->treeWidget())),
          dialog(nullptr),
          data(data)
    {
        this->setIcon(0, icon);
    }

    void performAction() override
    {
        if(dialog == nullptr)
        {
            dialog = new DialogType(parent);
            dialog->setData(data);

            QObject::connect(dialog, &DialogType::modified, [&]{
                data = dialog->getData();
                parent->modified();
            });

            QObject::connect(dialog, &DialogType::rejected, [&]{
                data = backup;
                parent->modified();
            });
        }

        backup = data;
        dialog->setData(data);
        dialog->activateWindow();
        dialog->show();
    }

private:
    TreeEditor* parent;
    DialogType* dialog;
    DataType& data;
    DataType backup;
};
