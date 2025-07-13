#include "StringSelectionView.hpp"

StringSelectionView::StringSelectionView(QAbstractItemModel* model, QPersistentModelIndex index, const QStringList& texts) {
    // Add items to combo box using the texts and values (user data) above.
    addItems(texts);

    // Select item whose user data matches the current value in the model and keep model up to date on changes
    setCurrentText(model->data(index).toString());
    QObject::connect(this, &QComboBox::currentIndexChanged, this, [=](int i){
        model->setData(index, itemText(i));
    });
}
