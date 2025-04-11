#pragma once
#include <QComboBox>

class StringSelectionView: public QComboBox {
    Q_OBJECT

public:
    StringSelectionView(QAbstractItemModel* model, QPersistentModelIndex index, const QStringList& texts) {
        // Add items to combo box using the texts and values (user data) above.
        addItems(texts);

        // Select item whose user data matches the current value in the model and keep model up to date on changes
        QObject::connect(this, &QComboBox::currentIndexChanged, this, [=](int i){ model->setData(index, itemText(i)); });
        setCurrentText(model->data(index).toString());
    }
};
