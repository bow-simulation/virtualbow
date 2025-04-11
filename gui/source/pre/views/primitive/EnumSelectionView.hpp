#pragma once
#include <QComboBox>

class EnumSelectionView: public QComboBox {
    Q_OBJECT

public:
    template<typename EnumType>
    EnumSelectionView(QAbstractItemModel* model, QPersistentModelIndex index, const QStringList& texts, const QList<EnumType>& values) {
        if(texts.size() != values.size()) {
            throw std::invalid_argument("Number of label texts and enum values must be equal");
        }

        // Add items to combo box using the texts and values (user data) above.
        for(int i = 0; i < texts.size(); ++i) {
            addItem(texts[i], static_cast<int>(values[i]));
        }

        // Select item whose user data matches the current value in the model and keep model up to date on changes
        QObject::connect(this, &QComboBox::currentIndexChanged, this, [=](int i){ model->setData(index, itemData(i)); });
        for(int i = 0; i < texts.size(); ++i) {
            if(itemData(i) == model->data(index, Qt::DisplayRole)) {
                setCurrentIndex(i);
            }
        }
    }
};
