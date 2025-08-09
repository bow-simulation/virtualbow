#include "StringSelectionView.hpp"

StringSelectionView::StringSelectionView(QAbstractItemModel* model, QPersistentModelIndex index, const QString& tooltip, const QStringList& texts, const QStringList& tooltips) {
    setToolTip(tooltip);

    // Add items to combo box using the texts and tooltips.
    for(int i = 0; i < texts.size(); ++i) {
        addItem(texts[i]);
        setItemData(i, tooltips[i], Qt::ToolTipRole);
    }

    // Select item whose user data matches the current value in the model and keep model up to date on changes
    setCurrentText(model->data(index).toString());
    QObject::connect(this, &QComboBox::currentIndexChanged, this, [=](int i){
        model->setData(index, itemText(i));
    });
}
