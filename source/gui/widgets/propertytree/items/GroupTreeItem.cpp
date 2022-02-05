#include "gui/widgets/propertytree/PropertyTreeItem.hpp"

class GroupTreeItem: public PropertyTreeItem {
public:
    GroupTreeItem(QString name, GroupTreeItem* parent = nullptr)
        : PropertyTreeItem(parent)
    {
        QFont font;
        font.setBold(true);

        this->setFont(0, font);
        this->setText(0, name);
    }
};
