#pragma once
#include "TreeEditor.hpp"

class Action
{
public:
    virtual ~Action() = default;
    virtual void performAction() = 0;
};

class TreeEditor;

template<class DialogType, class DataType>
class TreeItem: public QTreeWidgetItem, public Action
{
public:
    TreeItem(TreeEditor* parent, const UnitSystem& units, DataType& data, const QString& name, const QIcon& icon)
        : QTreeWidgetItem(parent, {name}),
          parent(parent),
          dialog(nullptr),
          units(units),
          data(data)
    {
        this->setIcon(0, icon);
    }

    void performAction() override
    {
        if(dialog == nullptr)
        {
            dialog = new DialogType(parent, units);
            dialog->setData(data);

            QObject::connect(dialog, &DialogType::modified, [&]{
                data = dialog->getData();
                parent->modified();
            });

            QObject::connect(dialog, &DialogType::rejected, [&]{
                if(data != backup)
                {
                    data = backup;
                    parent->modified();
                }
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

    const UnitSystem& units;
    DataType& data;
    DataType backup;
}; 
