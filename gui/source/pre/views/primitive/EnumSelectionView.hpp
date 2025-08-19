#pragma once
#include <QComboBox>

template<typename EnumType>
struct EnumItem {
    EnumType value;
    QString name;
    QString tooltip;
};

class EnumSelectionView: public QComboBox {
    Q_OBJECT

public:
    template<typename EnumType>
    EnumSelectionView(QAbstractItemModel* model, QPersistentModelIndex index, const QList<EnumItem<EnumType>>& items, const QString& tooltip) {
        setToolTip(tooltip);

        // Add items to combo box using the texts and values (user data) above.
        for(auto& item: items) {
            addItem(item.name, static_cast<int>(item.value));
            setItemData(count() - 1, item.tooltip, Qt::ToolTipRole);
        }

        // Select item whose user data matches the current value in the model
        for(int i = 0; i < items.size(); ++i) {
            if(itemData(i) == model->data(index, Qt::DisplayRole)) {
                setCurrentIndex(i);
            }
        }

        // Keep model up to date on changes
        QObject::connect(this, &QComboBox::currentIndexChanged, this, [=, this](int i){
            model->setData(index, itemData(i));
        });
    }
};
