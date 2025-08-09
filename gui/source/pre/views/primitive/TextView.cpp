#include "TextView.hpp"
#include <QAbstractItemModel>

TextView::TextView(QAbstractItemModel* model, QPersistentModelIndex index) {
    setWordWrapMode(QTextOption::NoWrap);
    setPlaceholderText("Empty");

    // Set value from model and keep model up to date on changes
    setPlainText(model->data(index, Qt::DisplayRole).toString());
    QObject::connect(this, &CustomTextEdit::editingFinished, this, [=]{
        model->setData(index, toPlainText());
    });
}
